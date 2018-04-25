#include "conct_runtime_high.h"

#include "conct_command.h"
#include "conct_device.h"
#include "conct_port.h"
#include "conct_proxy.h"
#include "conct_reader.h"
#include "conct_runtime.h"
#include "conct_writer.h"

namespace conct
{
	void RuntimeHigh::setup( Device* pDevice )
	{
		ArrayView< LocalInstance > instances;
		pDevice->getInstances( instances );

		for( uint i = 0u; i < instances.getCount(); ++i )
		{
			m_instances.push_back( instances[ i ] );
		}
	}

	void RuntimeHigh::registerPort( Port* pPort )
	{
		PortData& portData = m_ports[ pPort ];

		const Flags8< PortFlag > flags = pPort->getFlags();
		if( flags.isSet( PortFlag_SingleEndpoint ) )
		{
			const DeviceId nextDeviceId = DeviceId( m_devices.size() + 1u );
			DeviceData& deviceData = m_devices[ nextDeviceId ];
			deviceData.nextCommandId	= FirstCommandId;
			deviceData.ownDeviceId		= 1u;
			deviceData.pTargetPort		= pPort;
		}

		if( !flags.isSet( PortFlag_Reliable ) )
		{
			// not supported
		}
	}

	void RuntimeHigh::processPort( Port* pPort )
	{
		PortData& portData = m_ports[ pPort ];

		readPort( pPort, portData );
		writePort( pPort, portData );

		processPackages( portData );


// 		if( m_state == State_ExecuteCommand )
// 		{
// 			executeCommand();
// 		}
//
// 		if( m_state == State_SendResponse )
// 		{
// 			sendData( pPort );
// 		}
// 		else
// 		{
// 			processData( pPort );
// 		}
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
		baseHeader.payloadSize		= uint16( payload.getCount() );
		baseHeader.commandId		= pCommand->getId();
		baseHeader.messageType		= messageType;
		baseHeader.messageResult	= ResultId_Success;

		SendPackage package;
		package.targetDeviceId	= deviceAddress.address[ 0u ];
		package.currentOffset	= 0u;

		const uintreg packageSize = sizeof( baseHeader ) + baseHeader.sourceHops + baseHeader.destinationHops + payload.getCount();
		package.data.reserve( packageSize );

		const uint8* pHeaderData = ( const uint8* )&baseHeader;
		for( uintreg i = 0u; i < sizeof( baseHeader ); ++i )
		{
			package.data.push_back( pHeaderData[ i ] );
		}

		package.data.push_back( deviceData.ownDeviceId );

		for( uintreg i = 0u; i < baseHeader.destinationHops; ++i )
		{
			package.data.push_back( deviceAddress.address[ i ] );
		}

		for( uintreg i = 0u; i < payload.getCount(); ++i )
		{
			package.data.push_back( payload[ i ] );
		}

		PortData& portData = m_ports[ deviceData.pTargetPort ];
		portData.sendPackages.push( package );

		deviceData.commands[ baseHeader.commandId ] = pCommand;

		return ResultId_Success;
	}

	void RuntimeHigh::processPackages( PortData& portData )
	{
		for( const ReceivedPackage& package : portData.receivedPackages )
		{
			if( package.destinationAddress.size() > 1u )
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
		const DeviceId nextDevice = sourcePackage.destinationAddress.front();
		DeviceMap::iterator deviceIt = m_devices.find( nextDevice );
		if( deviceIt == m_devices.end() )
		{
			// target not found. send error to source!
			return;
		}

		const DeviceData& targetDevice = deviceIt->second;
		PortData& targetPortData = m_ports[ targetDevice.pTargetPort ];

		MessageBaseHeader baseHeader = sourcePackage.baseHeader;
		baseHeader.destinationHops--;
		baseHeader.sourceHops++;

		SendPackage package;
		package.targetDeviceId	= nextDevice;
		package.currentOffset	= 0u;

		const uintreg packageSize = sizeof( baseHeader ) + baseHeader.sourceHops + baseHeader.destinationHops + sourcePackage.payload.size();
		package.data.reserve( packageSize );

		const uint8* pHeaderData = ( const uint8* )&baseHeader;
		for( uintreg i = 0u; i < sizeof( baseHeader ); ++i )
		{
			package.data.push_back( pHeaderData[ i ] );
		}

		package.data.push_back( targetDevice.ownDeviceId );
		for( DeviceId deviceId : sourcePackage.sourceAddress )
		{
			package.data.push_back( deviceId );
		}

		for( uintreg i = 1u; i < sourcePackage.destinationAddress.size(); ++i )
		{
			package.data.push_back( sourcePackage.destinationAddress[ i ] );
		}

		for( uint8 byte : sourcePackage.payload )
		{
			package.data.push_back( byte );
		}


		portData.sendPackages.push( package );
	}

	void RuntimeHigh::processPackage( PortData& portData, const ReceivedPackage& package )
	{
		DeviceData& deviceData = m_devices[ package.sourceAddress.front() ];

		CommandBase* pCommandBase = nullptr;
		if( package.baseHeader.commandId != InvalidCommandId )
		{
			pCommandBase = deviceData.commands[ package.baseHeader.commandId ];
		}

		switch( package.baseHeader.messageType )
		{
		case MessageType_ErrorResponse:
			break;

		case MessageType_PingRequest:
			break;

		case MessageType_PingResponse:
			break;

		case MessageType_GetInstanceRequest:
			break;

		case MessageType_GetInstanceResponse:
			{
				if( pCommandBase == nullptr )
				{
					// error
					return;
				}

				const GetInstanceResponse& response = *( const GetInstanceResponse* )package.payload.data();

				RemoteInstance instance;
				//instance.address	= package.sourceAddress;
				instance.id			= response.instanceId;

				Command< RemoteInstance >* pCommand = static_cast< Command< RemoteInstance >* >( pCommandBase );
				pCommand->setResponse( package.baseHeader.messageResult, instance );
			}
			break;

		case MessageType_GetPropertyRequest:
			break;

		case MessageType_GetPropertyResponse:
			break;

		case MessageType_SetPropertyRequest:
			break;

		case MessageType_SetPropertyResponse:
			break;

		case MessageType_CallFunctionRequest:
			break;

		case MessageType_CallFunctionResponse:
			break;

		default:
			break;
		}
	}

	void RuntimeHigh::readPort( Port* pPort, PortData& portData )
	{
		Reader reader;
		DeviceId deviceId;
		while( pPort->openReceived( reader, deviceId ) )
		{
			readPackage( pPort, portData, reader, deviceId );
			pPort->closeReceived( reader );
		}
	}

	void RuntimeHigh::readPackage( Port* pPort, PortData& portData, Reader& reader, DeviceId deviceId )
	{
		PendingReceivedPackage& package = portData.pendingPackages[ deviceId ];
		while( !reader.isEnd() )
		{
			switch( package.state )
			{
			//case PackageState_WaitForMagic:
			//	readMagic( package, reader );
			//	break;

			case PackageState_ReadBaseHeader:
				readBaseHeader( package, reader );
				break;

			case PackageState_ReadSourceAddress:
				readBytes( package.target.sourceAddress, package, reader, PackageState_ReadDestinationAddress );
				break;

			case PackageState_ReadDestinationAddress:
				readBytes( package.target.destinationAddress, package, reader, PackageState_ReadPayload );
				break;

			case PackageState_ReadPayload:
				readBytes( package.target.payload, package, reader, PackageState_PushToQueue );
				break;

			case PackageState_PushToQueue:
				break;
			}

			if( package.state == PackageState_PushToQueue )
			{
				readStore( pPort, portData, package, deviceId );
			}
		}
	}

	//void RuntimeHigh::readMagic( PendingReceivedPackage& package, Reader& reader )
	//{
	//	if( package.data.waitForMagic.firstReadCounter < sizeof( s_messageBaseHeaderMagic ) )
	//	{
	//		package.data.waitForMagic.firstReadCounter += reader.readShort( package.data.waitForMagic.lastMagic, package.data.waitForMagic.firstReadCounter );

	//		if( package.data.waitForMagic.firstReadCounter == sizeof( s_messageBaseHeaderMagic ) &&
	//			package.data.waitForMagic.lastMagic == s_messageBaseHeaderMagic )
	//		{
	//			setState( package, PackageState_ReadBaseHeader );
	//			return;
	//		}
	//	}

	//	uint8 nextByte;
	//	while( reader.readByte( nextByte ) )
	//	{
	//		package.data.waitForMagic.lastMagic <<= 8u;
	//		package.data.waitForMagic.lastMagic |= nextByte;

	//		if( package.data.waitForMagic.lastMagic == s_messageBaseHeaderMagic )
	//		{
	//			setState( package, PackageState_ReadBaseHeader );
	//			return;
	//		}
	//	}
	//}

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

		package.target.sourceAddress.resize( package.target.baseHeader.sourceHops );
		package.target.destinationAddress.resize( package.target.baseHeader.destinationHops );
		package.target.payload.resize( package.target.baseHeader.payloadSize );

		setState( package, PackageState_ReadSourceAddress );
	}

	void RuntimeHigh::readBytes( std::vector<uint8>& target, PendingReceivedPackage& package, Reader& reader, PackageState nextState )
	{
		package.data.readBytes.alreadyRead += reader.readData( target.data(), (uintreg)target.size(), package.data.readBytes.alreadyRead );
		if( package.data.readBytes.alreadyRead == target.size() )
		{
			setState( package, nextState );
		}
	}

	void RuntimeHigh::readStore( Port* pPort, PortData& portData, PendingReceivedPackage& package, DeviceId deviceId )
	{
		package.target.sourceAddress.insert( package.target.sourceAddress.begin(), deviceId );
		portData.receivedPackages.push_back( package.target );

		DeviceMap::iterator deviceIt = m_devices.find( deviceId );
		if( deviceIt == m_devices.end() )
		{
			DeviceData& device = m_devices[ deviceId ];

			device.pTargetPort	= pPort;
			device.ownDeviceId	= *package.target.destinationAddress.begin();
		}

		setState( package, PackageState_ReadBaseHeader );
	}

	void RuntimeHigh::writePort( Port* pPort, PortData& portData )
	{
		if( portData.sendPackages.empty() )
		{
			return;
		}

		SendPackage& currentPackage = portData.sendPackages.front();

		Writer writer;
		const uintreg remainingSize = currentPackage.data.size() - currentPackage.currentOffset;
		if( !pPort->openSend( writer, remainingSize, currentPackage.targetDeviceId ) )
		{
			return;
		}

		uintreg index = currentPackage.currentOffset;
		while( !writer.isEnd() && index < currentPackage.data.size() )
		{
			writer.writeByte( currentPackage.data[ index++ ] );
		}

		if( index == currentPackage.data.size() )
		{
			portData.sendPackages.pop();
		}
		else
		{
			currentPackage.currentOffset = index;
		}

		pPort->closeSend( writer );
	}

	void RuntimeHigh::setState( PendingReceivedPackage& package, PackageState state )
	{
		package.state = state;

		switch( state )
		{
		//case PackageState_WaitForMagic:
		//	package.data.waitForMagic.firstReadCounter = 0u;
		//	package.data.waitForMagic.lastMagic = 0u;
		//	break;

		case PackageState_ReadBaseHeader:
		case PackageState_ReadSourceAddress:
		case PackageState_ReadDestinationAddress:
		case PackageState_ReadPayload:
			package.data.readBytes.alreadyRead = 0u;
			break;
		}
	}
}