#include "conct_runtime_high.h"

#include "conct_buffered_value_builder.h"
#include "conct_command.h"
#include "conct_device.h"
#include "conct_port.h"
#include "conct_proxy.h"
#include "conct_reader.h"
#include "conct_runtime.h"
#include "conct_value_high.h"
#include "conct_writer.h"

namespace conct
{
	void RuntimeHigh::setup( Device* pDevice )
	{
		m_pDevice		= pDevice;
		m_nextDeviceId	= FirstDeviceId;
	}

	void RuntimeHigh::registerPort( Port* pPort )
	{
		m_ports[ pPort ];
	}

	void RuntimeHigh::unregisterPort( Port* pPort )
	{
		PortData& portData = m_ports[ pPort ];

		for( PortData::EndpointDeviceMap::PairType& kvp : portData.endpointToDevice )
		{
			m_devices.erase( kvp.value.deviceId );
		}

		m_ports.erase( pPort );
	}

	void RuntimeHigh::processPort( Port* pPort )
	{
		PortData& portData = m_ports[ pPort ];

		uintreg endpointId;
		while( pPort->popConnectionReset( endpointId ) )
		{
			setState( portData.pendingPackages[ endpointId ], PackageState_ReadBaseHeader );

			const EndpointData& endpointData = portData.endpointToDevice[ endpointId ];
			DeviceData& deviceData = m_devices[ endpointData.deviceId ];

			for( DeviceData::CommandMap::PairType& pair : deviceData.commands )
			{
				m_finishCommands.pushBack( pair.value );
				pair.value->setResponse( ResultId_ConnectionLost );
			}

			m_devices.erase( endpointData.deviceId );
			portData.endpointToDevice.erase( endpointId );
		}

		ArrayView< uintreg > endpoints;
		pPort->getEndpoints( endpoints );
		for( uint i = 0u; i < endpoints.getLength(); ++i )
		{
			const EndpointData* pEndpointData;
			if( portData.endpointToDevice.find( pEndpointData, endpoints[ i ] ) )
			{
				continue;
			}

			addDevice( pPort, portData, InvalidDeviceId, endpoints[ i ] );
		}

		readPort( pPort, portData );
		writePort( pPort, portData );

		processPackages( portData );
	}

	uintreg RuntimeHigh::getDeviceCount() const
	{
		return m_devices.getLength();
	}

	void RuntimeHigh::getDevices( Vector< DeviceId >& devices ) const
	{
		devices.reserve( m_devices.getLength() );
		for( const DeviceMap::PairType& kvp : m_devices )
		{
			devices.pushBack( kvp.key );
		}
	}

	bool RuntimeHigh::isThisDevice( const DeviceAddress& address ) const
	{
		if( address.address[ 0u ] == InvalidDeviceId )
		{
			return false;
		}

		const DeviceData* pDeviceData = nullptr;
		if( !m_devices.find( pDeviceData, address.address[ 0u ] ) )
		{
			return false;
		}

		if( pDeviceData->ownDeviceId == InvalidDeviceId ||
			address.address[ 1u ] == InvalidDeviceId )
		{
			return false;
		}

		return pDeviceData->ownDeviceId == address.address[ 1u ];
	}

	CommandId RuntimeHigh::getNextCommandId( DeviceId deviceId )
	{
		DeviceData* pDeviceData = nullptr;
		if( !m_devices.find( pDeviceData, deviceId ) )
		{
			return InvalidCommandId;
		}

		const CommandId commandId = pDeviceData->nextCommandId;
		while( ++pDeviceData->nextCommandId == InvalidCommandId );

		return commandId;
	}

	ResultId RuntimeHigh::sendPackage( Command* pCommand, const DeviceAddress& deviceAddress, const ArrayView< uint8 >& payload, MessageType messageType )
	{
		const ResultId result = sendPackage( deviceAddress, payload, pCommand->getId(), messageType, ResultId_Success );
		if( result == ResultId_Success )
		{
			DeviceData& deviceData = m_devices[ deviceAddress.address[ 0u ] ];
			deviceData.commands[ pCommand->getId() ] = pCommand;
			pCommand->setSent();
		}

		return result;
	}

	bool RuntimeHigh::popFinishCommand( Command*& pCommand )
	{
		return m_finishCommands.tryPopFront( pCommand );
	}

	void RuntimeHigh::processPackages( PortData& portData )
	{
		for( const ReceivedPackage& package : portData.receivedPackages )
		{
			if( package.destinationAddress.getLength() > 1u )
			{
				processRoute( portData, package );
			}
			else
			{
				processPackage( portData, package );
			}
		}
		portData.receivedPackages.clear();
	}

	void RuntimeHigh::processRoute( PortData& portData, const ReceivedPackage& sourcePackage )
	{
		const DeviceId nextDeviceId = sourcePackage.destinationAddress[ 1u ];

		DeviceData* pTargetDeviceData = nullptr;
		if( !m_devices.find( pTargetDeviceData, nextDeviceId ) )
		{
			sendErrorResponse( sourcePackage, ResultId_NoSuchDevice );
			return;
		}

		MessageBaseHeader baseHeader = sourcePackage.baseHeader;
		baseHeader.destinationHops--;
		baseHeader.sourceHops++;

		SendPackage package;
		package.targetEndpointId	= pTargetDeviceData->endpointId;
		package.currentOffset		= 0u;

		const uintreg packageSize = sizeof( baseHeader ) + baseHeader.sourceHops + baseHeader.destinationHops + sourcePackage.payload.getLength();
		package.data.reserve( packageSize );

		const uint8* pHeaderData = ( const uint8* )&baseHeader;
		package.data.pushRange( pHeaderData, sizeof( baseHeader ) );

		package.data.pushBack( pTargetDeviceData->ownDeviceId );
		package.data.pushBack( sourcePackage.deviceId );
		package.data.pushRange( sourcePackage.sourceAddress.getData() + 1u, sourcePackage.sourceAddress.getLength() - 1u );
		package.data.pushRange( sourcePackage.destinationAddress.getData() + 1u, sourcePackage.destinationAddress.getLength() - 1u );
		package.data.pushRange( sourcePackage.payload );

		PortData& targetPortData = m_ports[ pTargetDeviceData->pTargetPort ];
		targetPortData.sendPackages.pushBack( package );
	}

	void RuntimeHigh::processPackage( PortData& portData, const ReceivedPackage& package )
	{
		Command* pCommand = nullptr;
		if( package.baseHeader.commandId != InvalidCommandId &&
			( package.baseHeader.messageType == MessageType_GetPropertyResponse ||
			package.baseHeader.messageType == MessageType_SetPropertyResponse ||
			package.baseHeader.messageType == MessageType_CallFunctionResponse ) )
		{
			DeviceData& deviceData = m_devices[ package.deviceId ];
			pCommand = deviceData.commands[ package.baseHeader.commandId ];
			deviceData.commands.erase( package.baseHeader.commandId );

			if( pCommand == nullptr )
			{
				// error
				return;
			}

			m_finishCommands.pushBack( pCommand );
		}

		switch( package.baseHeader.messageType )
		{
		case MessageType_ErrorResponse:
			{
				if( pCommand != nullptr )
				{
					pCommand->setResponse( package.baseHeader.messageResult );
				}
			}
			break;

		case MessageType_PingRequest:
			{
				sendResponse( package, ArrayView< uint8 >(), MessageType_PingResponse );
			}
			break;

		case MessageType_PingResponse:
			break;

		case MessageType_GetPropertyRequest:
			{
				BufferedValueBuilder< 1024u > valueBuilder;
				{
					const GetPropertyRequest& request = *reinterpret_cast< const GetPropertyRequest* >( package.payload.getData() );

					const LocalInstance* pInstance = m_pDevice->getInstance( request.instanceId );
					if( pInstance == nullptr )
					{
						sendErrorResponse( package, ResultId_NoSuchInstance );
						return;
					}

					if( !pInstance->pProxy->getProperty( valueBuilder, pInstance->pInstance, request.name ) )
					{
						sendErrorResponse( package, ResultId_NoSuchField );
						return;
					}
				}

				sendResponse( package, valueBuilder.toArrayView(), MessageType_GetPropertyResponse );
			}
			break;

		case MessageType_GetPropertyResponse:
			{
				const GetPropertyResponse& response = *( const GetPropertyResponse* )package.payload.getData();

				ValueCommand* pValueCommand = static_cast< ValueCommand* >( pCommand );
				pValueCommand->setResponse( package.baseHeader.messageResult, response.value );
			}
			break;

		case MessageType_SetPropertyRequest:
			{
				const SetPropertyRequest& request = *reinterpret_cast< const SetPropertyRequest* >( package.payload.getData() );

				const LocalInstance* pInstance = m_pDevice->getInstance( request.instanceId );
				if( pInstance == nullptr )
				{
					sendErrorResponse( package, ResultId_NoSuchInstance );
					return;
				}

				if( !pInstance->pProxy->setProperty( pInstance->pInstance, request.name, request.value ) )
				{
					sendErrorResponse( package, ResultId_NoSuchField );
					return;
				}

				sendResponse( package, ArrayView< uint8 >(), MessageType_SetPropertyResponse );
			}
			break;

		case MessageType_SetPropertyResponse:
			{
				pCommand->setResponse( package.baseHeader.messageResult );
			}
			break;

		case MessageType_CallFunctionRequest:
			{
				BufferedValueBuilder< 1024u > valueBuilder;
				{
					const CallFunctionRequest& request = *reinterpret_cast< const CallFunctionRequest* >( package.payload.getData() );

					const LocalInstance* pInstance = m_pDevice->getInstance( request.instanceId );
					if( pInstance == nullptr )
					{
						sendErrorResponse( package, ResultId_NoSuchInstance );
						return;
					}

					if( !pInstance->pProxy->callFunction( valueBuilder, pInstance->pInstance, request.name, request.arguments.toView() ) )
					{
						sendErrorResponse( package, ResultId_NoSuchField );
						return;
					}
				}

				sendResponse( package, valueBuilder.toArrayView(), MessageType_CallFunctionResponse );
			}
			break;

		case MessageType_CallFunctionResponse:
			{
				const CallFunctionResponse& response = *( const CallFunctionResponse* )package.payload.getData();

				ValueCommand* pValueCommand = static_cast< ValueCommand* >( pCommand );
				pValueCommand->setResponse( package.baseHeader.messageResult, response.value );
			}
			break;

		default:
			break;
		}
	}

	DeviceId RuntimeHigh::addDevice( Port* pPort, PortData& portData, DeviceId ownDeviceId, uintreg endpointId )
	{
		if( m_nextDeviceId == InvalidDeviceId )
		{
			return InvalidDeviceId;
		}

		const DeviceId nextDeviceId = m_nextDeviceId;
		m_nextDeviceId++;

		DeviceData& deviceData = m_devices[ nextDeviceId ];
		deviceData.nextCommandId	= FirstCommandId;
		deviceData.endpointId		= endpointId;
		deviceData.ownDeviceId		= ownDeviceId;
		deviceData.pTargetPort		= pPort;

		EndpointData endpointData;
		endpointData.endpointId	= endpointId;
		//endpointData.mode		= EndpointMode_Full;
		endpointData.deviceId	= nextDeviceId;
		portData.endpointToDevice[ endpointId ] = endpointData;

		return nextDeviceId;
	}

	void RuntimeHigh::readPort( Port* pPort, PortData& portData )
	{
		Reader reader;
		uintreg endpointId;
		while( pPort->openReceived( reader, endpointId ) )
		{
			readPackage( pPort, portData, reader, endpointId );
			pPort->closeReceived( reader, endpointId );
		}
	}

	void RuntimeHigh::readPackage( Port* pPort, PortData& portData, Reader& reader, uintreg endpointId )
	{
		PendingReceivedPackage& package = portData.pendingPackages[ endpointId ];
		while( !reader.isEnd() )
		{
			if( package.state == PackageState_ReadBaseHeader )
			{
				readBaseHeader( package, reader );
			}

			if( package.state == PackageState_ReadSourceAddress )
			{
				readBytes( package.target.sourceAddress, package, reader, PackageState_ReadDestinationAddress );
			}

			if( package.state == PackageState_ReadDestinationAddress )
			{
				readBytes( package.target.destinationAddress, package, reader, PackageState_ReadPayload );
			}

			if( package.state == PackageState_ReadPayload )
			{
				readBytes( package.target.payload, package, reader, PackageState_PushToQueue );
			}

			if( package.state == PackageState_PushToQueue )
			{
				readStore( pPort, portData, package, endpointId );
			}
		}
	}

	void RuntimeHigh::readBaseHeader( PendingReceivedPackage& package, Reader& reader )
	{
		package.data.readBytes.alreadyRead += reader.readStruct( package.target.baseHeader, package.data.readBytes.alreadyRead );
		if( package.data.readBytes.alreadyRead < sizeof( package.target.baseHeader ) )
		{
			return;
		}

		if( package.target.baseHeader.destinationHops < 1u )
		{
			// no answer id
		}

		package.target.sourceAddress.setLength( package.target.baseHeader.sourceHops );
		package.target.destinationAddress.setLength( package.target.baseHeader.destinationHops );
		package.target.payload.setLength( package.target.baseHeader.payloadSize );

		setState( package, PackageState_ReadSourceAddress );
	}

	void RuntimeHigh::readBytes( Vector<uint8>& target, PendingReceivedPackage& package, Reader& reader, PackageState nextState )
	{
		package.data.readBytes.alreadyRead += reader.readData( target.getData(), target.getLength(), package.data.readBytes.alreadyRead );
		if( package.data.readBytes.alreadyRead == target.getLength() )
		{
			setState( package, nextState );
		}
	}

	void RuntimeHigh::readStore( Port* pPort, PortData& portData, PendingReceivedPackage& package, uintreg endpointId )
	{
		const DeviceId ownDeviceId = package.target.destinationAddress.getFront();

		EndpointData* pEndpointData = nullptr;
		if( portData.endpointToDevice.find( pEndpointData, endpointId ) )
		{
			const DeviceId deviceId = pEndpointData->deviceId;
			DeviceData& deviceData = m_devices[ deviceId ];

			CONCT_ASSERT( deviceData.ownDeviceId == InvalidDeviceId || deviceData.ownDeviceId == ownDeviceId );
			deviceData.ownDeviceId = ownDeviceId;

			package.target.deviceId = deviceId;
		}
		else
		{
			const DeviceId deviceId = addDevice( pPort, portData, ownDeviceId, endpointId );
			package.target.deviceId = deviceId;
		}

		portData.receivedPackages.pushBack( package.target );

		setState( package, PackageState_ReadBaseHeader );
	}

	void RuntimeHigh::writePort( Port* pPort, PortData& portData )
	{
		if( portData.sendPackages.isEmpty() )
		{
			return;
		}

		SendPackage& currentPackage = portData.sendPackages.getFront();

		Writer writer;
		const uintreg remainingSize = currentPackage.data.getLength() - currentPackage.currentOffset;
		if( !pPort->openSend( writer, remainingSize, currentPackage.targetEndpointId ) )
		{
			return;
		}

		while( !writer.isEnd() && currentPackage.currentOffset < currentPackage.data.getLength() )
		{
			writer.writeByte( currentPackage.data[ currentPackage.currentOffset++ ] );
		}

		pPort->closeSend( writer, currentPackage.targetEndpointId );

		if( currentPackage.currentOffset == currentPackage.data.getLength() )
		{
			portData.sendPackages.popFront();
		}
	}

	void RuntimeHigh::setState( PendingReceivedPackage& package, PackageState state )
	{
		package.state = state;

		switch( state )
		{
		case PackageState_ReadBaseHeader:
		case PackageState_ReadSourceAddress:
		case PackageState_ReadDestinationAddress:
		case PackageState_ReadPayload:
			package.data.readBytes.alreadyRead = 0u;
			break;

		case PackageState_PushToQueue:
			break;
		}
	}

	void RuntimeHigh::getDeviceAddress( DeviceAddress& targetAddress, DeviceId targetDeviceId, const Vector< DeviceId >& sourceAddress ) const
	{
		targetAddress.address[ 0u ] = targetDeviceId;

		for( uintreg i = 1u; i < sourceAddress.getLength(); ++i )
		{
			targetAddress.address[ i ] = sourceAddress[ i ];
		}

		targetAddress.address[ sourceAddress.getLength() ] = InvalidDeviceId;
	}

	ResultId RuntimeHigh::sendPackage( const DeviceAddress& deviceAddress, const ArrayView< uint8 >& payload, CommandId commandId, MessageType messageType, ResultId result )
	{
		MessageBaseHeader baseHeader;

		baseHeader.destinationHops = 0u;
		for( uintreg i = 0u; i < DeviceAddress::Size; ++i )
		{
			if( deviceAddress.address[ i ] == InvalidDeviceId )
			{
				break;
			}

			baseHeader.destinationHops++;
		}

		if( baseHeader.destinationHops == 0u )
		{
			return ResultId_NoDestination;
		}

		DeviceData* pDeviceData = nullptr;
		if( !m_devices.find( pDeviceData, deviceAddress.address[ 0u ] ) )
		{
			return ResultId_NoSuchDevice;
		}

		baseHeader.sourceHops		= 1u;
		baseHeader.payloadSize		= uint16( payload.getLength() );
		baseHeader.commandId		= commandId;
		baseHeader.messageType		= messageType;
		baseHeader.messageResult	= result;

		SendPackage package;
		package.targetEndpointId	= pDeviceData->endpointId;
		package.currentOffset		= 0u;

		const uintreg packageSize = sizeof( baseHeader ) + baseHeader.sourceHops + baseHeader.destinationHops + payload.getLength();
		package.data.reserve( packageSize );

		const uint8* pHeaderData = ( const uint8* )&baseHeader;
		package.data.pushRange( pHeaderData, sizeof( baseHeader ) );

		package.data.pushBack( pDeviceData->ownDeviceId );

		for( uintreg i = 0u; i < baseHeader.destinationHops; ++i )
		{
			package.data.pushBack( deviceAddress.address[ i ] );
		}

		package.data.pushRange( payload );

		PortData& portData = m_ports[ pDeviceData->pTargetPort ];
		portData.sendPackages.pushBack( package );

		return ResultId_Success;
	}

	ResultId RuntimeHigh::sendResponse( const ReceivedPackage& package, const ArrayView< uint8 >& payload, MessageType messageType )
	{
		DeviceAddress address;
		getDeviceAddress( address, package.deviceId, package.sourceAddress );

		return sendPackage( address, payload, package.baseHeader.commandId, messageType, ResultId_Success );
	}

	ResultId RuntimeHigh::sendErrorResponse( const ReceivedPackage& package, ResultId result )
	{
		DeviceAddress address;
		getDeviceAddress( address, package.deviceId, package.sourceAddress );

		return sendPackage( address, ArrayView< uint8 >(), package.baseHeader.commandId, MessageType_ErrorResponse, result );
	}
}