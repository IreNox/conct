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
		m_pDevice = pDevice;
	}

	void RuntimeHigh::registerPort( Port* pPort )
	{
		PortData& portData = m_ports[ pPort ];

		const Flags8< PortFlag > flags = pPort->getFlags();
		if( flags.isSet( PortFlag_SingleEndpoint ) )
		{
			const DeviceId deviceId = addDevice( pPort, 1u, 0u );
			portData.endpointToDevice[ 0u ] = deviceId;
		}

		if( !flags.isSet( PortFlag_Reliable ) )
		{
			// not supported
		}
	}

	void RuntimeHigh::unregisterPort( Port* pPort )
	{
		PortData& portData = m_ports[ pPort ];

		for( std::pair< const uintreg, DeviceId >& kvp : portData.endpointToDevice )
		{
			m_devices.erase( kvp.second );
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

			const DeviceId deviceId = portData.endpointToDevice[ endpointId ];
			DeviceData& deviceData = m_devices[ deviceId ];

			for( std::pair< const CommandId, CommandBase* >& pair : deviceData.commands )
			{
				pair.second->setResponse( ResultId_ConnectionLost );
			}
		}

		readPort( pPort, portData );
		writePort( pPort, portData );

		processPackages( portData );
	}

	void RuntimeHigh::getDevices( Vector< DeviceId >& devices ) const
	{
		devices.reserve( m_devices.size() );
		for( const std::pair< DeviceId, DeviceData >& kvp : m_devices )
		{
			devices.pushBack( kvp.first );
		}
	}

	CommandId RuntimeHigh::getNextCommandId( DeviceId deviceId )
	{
		DeviceMap::iterator it = m_devices.find( deviceId );
		if( it == m_devices.end() )
		{
			return InvalidCommandId;
		}

		const CommandId commandId = it->second.nextCommandId;
		while( ++it->second.nextCommandId == InvalidCommandId );

		return commandId;
	}

	ResultId RuntimeHigh::sendPackage( CommandBase* pCommand, const DeviceAddress& deviceAddress, const ArrayView< uint8 >& payload, MessageType messageType )
	{
		const ResultId result = sendPackage( deviceAddress, payload, pCommand->getId(), messageType, ResultId_Success );
		if( result == ResultId_Success )
		{
			DeviceMap::iterator it = m_devices.find( deviceAddress.address[ 0u ] );
			if( it == m_devices.end() )
			{
				return ResultId_NoSuchDevice;
			}
			DeviceData& deviceData = it->second;

			deviceData.commands[ pCommand->getId() ] = pCommand;
		}

		return result;
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
		DeviceMap::iterator deviceIt = m_devices.find( nextDeviceId );
		if( deviceIt == m_devices.end() )
		{
			// target not found. send error to source!
			return;
		}
		const DeviceData& targetDevice = deviceIt->second;

		MessageBaseHeader baseHeader = sourcePackage.baseHeader;
		baseHeader.destinationHops--;
		baseHeader.sourceHops++;

		SendPackage package;
		package.targetEndpointId	= targetDevice.endpointId;
		package.currentOffset		= 0u;

		const uintreg packageSize = sizeof( baseHeader ) + baseHeader.sourceHops + baseHeader.destinationHops + sourcePackage.payload.getLength();
		package.data.reserve( packageSize );

		const uint8* pHeaderData = ( const uint8* )&baseHeader;
		package.data.pushRange( pHeaderData, sizeof( baseHeader ) );

		package.data.pushBack( targetDevice.ownDeviceId );
		package.data.pushBack( sourcePackage.deviceId );
		package.data.pushRange( sourcePackage.sourceAddress.getData() + 1u, sourcePackage.sourceAddress.getLength() - 1u );
		package.data.pushRange( sourcePackage.destinationAddress.getData() + 1u, sourcePackage.destinationAddress.getLength() - 1u );
		package.data.pushRange( sourcePackage.payload );

		PortData& targetPortData = m_ports[ targetDevice.pTargetPort ];
		targetPortData.sendPackages.pushBack( package );
	}

	void RuntimeHigh::processPackage( PortData& portData, const ReceivedPackage& package )
	{
		CommandBase* pCommandBase = nullptr;
		if( package.baseHeader.commandId != InvalidCommandId &&
			( package.baseHeader.messageType == MessageType_GetPropertyResponse ||
			package.baseHeader.messageType == MessageType_SetPropertyResponse ||
			package.baseHeader.messageType == MessageType_CallFunctionResponse ) )
		{
			DeviceData& deviceData = m_devices[ package.deviceId ];
			pCommandBase = deviceData.commands[ package.baseHeader.commandId ];

			if( pCommandBase == nullptr )
			{
				// error
				return;
			}
		}

		switch( package.baseHeader.messageType )
		{
		case MessageType_ErrorResponse:
			{
				if( pCommandBase != nullptr )
				{
					pCommandBase->setResponse( package.baseHeader.messageResult );
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

				Command< ValueHigh >* pCommand = static_cast< Command< ValueHigh >* >( pCommandBase );
				pCommand->setResponse( package.baseHeader.messageResult, response.value );
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
				pCommandBase->setResponse( package.baseHeader.messageResult );
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

				Command< ValueHigh >* pCommand = static_cast< Command< ValueHigh >* >( pCommandBase );
				pCommand->setResponse( package.baseHeader.messageResult, response.value );
			}
			break;

		default:
			break;
		}
	}

	DeviceId RuntimeHigh::addDevice( Port* pPort, DeviceId ownDeviceId, uintreg endpointId )
	{
		const DeviceId nextDeviceId = DeviceId( m_devices.size() + 1u );

		DeviceData& deviceData = m_devices[ nextDeviceId ];
		deviceData.nextCommandId	= FirstCommandId;
		deviceData.endpointId		= endpointId;
		deviceData.ownDeviceId		= ownDeviceId;
		deviceData.pTargetPort		= pPort;

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
		PortData::EndpointDeviceMap::iterator endpointIt = portData.endpointToDevice.find( endpointId );
		if( endpointIt == portData.endpointToDevice.end() )
		{
			const DeviceId deviceId = addDevice( pPort, package.target.destinationAddress.getFront(), endpointId );
			portData.endpointToDevice[ endpointId ] = deviceId;

			package.target.deviceId = deviceId;
		}
		else
		{
			package.target.deviceId = endpointIt->second;
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

		uintreg index = currentPackage.currentOffset;
		while( !writer.isEnd() && index < currentPackage.data.getLength() )
		{
			writer.writeByte( currentPackage.data[ index++ ] );
		}

		if( index == currentPackage.data.getLength() )
		{
			portData.sendPackages.popFront();
		}
		else
		{
			currentPackage.currentOffset = index;
		}

		pPort->closeSend( writer, currentPackage.targetEndpointId );
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

		DeviceMap::iterator it = m_devices.find( deviceAddress.address[ 0u ] );
		if( it == m_devices.end() )
		{
			return ResultId_NoSuchDevice;
		}
		DeviceData& deviceData = it->second;

		baseHeader.sourceHops		= 1u;
		baseHeader.payloadSize		= uint16( payload.getLength() );
		baseHeader.commandId		= commandId;
		baseHeader.messageType		= messageType;
		baseHeader.messageResult	= result;

		SendPackage package;
		package.targetEndpointId	= deviceData.endpointId;
		package.currentOffset		= 0u;

		const uintreg packageSize = sizeof( baseHeader ) + baseHeader.sourceHops + baseHeader.destinationHops + payload.getLength();
		package.data.reserve( packageSize );

		const uint8* pHeaderData = ( const uint8* )&baseHeader;
		package.data.pushRange( pHeaderData, sizeof( baseHeader ) );

		package.data.pushBack( deviceData.ownDeviceId );

		for( uintreg i = 0u; i < baseHeader.destinationHops; ++i )
		{
			package.data.pushBack( deviceAddress.address[ i ] );
		}

		package.data.pushRange( payload );

		PortData& portData = m_ports[ deviceData.pTargetPort ];
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