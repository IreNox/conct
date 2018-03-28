#include "conct_runtime_high.h"

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

	void RuntimeHigh::processPort( Port* pPort )
	{
		PortData& portData = m_ports[ pPort ];

		readPort( pPort, portData );
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
			return ResultId_NoSuchDevice;
		}
	}

	ResultId RuntimeHigh::sendPackage( CommandBase* pCommand, const DeviceAddress& deviceAddress, const ArrayView< uint8 >& payload )
	{
		Package package;
		if( package.destinationAddress.empty() )
		{
			return ResultId_NoDestination;
		}

		DeviceMap::iterator it = m_devices.find( *package.destinationAddress.begin() );
		if( it == m_devices.end() )
		{
			return ResultId_NoSuchDevice;
		}

		for( uint i = 0u; i < payload.getCount(); ++i )
		{
			package.payload.push_back( payload[ i ] );
		}

		const DeviceData& deviceData = it->second;
		PortData& portData = m_ports[ deviceData.pTargetPort ];
		portData.sendPackages.push_back( package );

		return ResultId_Success;
	}

	void RuntimeHigh::processPackages( PortData& portData )
	{
		for( const Package& package : portData.receivedPackages )
		{
			if( package.destinationAddress.size() > 1u )
			{
				processRoute( portData, package );
			}
			else
			{
				// ...
			}
		}
		portData.receivedPackages.clear();
	}

	void RuntimeHigh::processRoute( PortData& portData, const Package& package )
	{
		const DeviceId nextDevice = *package.destinationAddress.begin();
		DeviceMap::iterator deviceIt = m_devices.find( nextDevice );
		if( deviceIt == m_devices.end() )
		{
			// target not found. send error to source!
			return;
		}

		const DeviceData& targetDevice = deviceIt->second;
		PortData& targetPortData = m_ports[ targetDevice.pTargetPort ];

		Package sendPackage = package;
		sendPackage.destinationAddress.erase( sendPackage.destinationAddress.begin() );
		sendPackage.sourceAddress.insert( sendPackage.sourceAddress.begin(), targetDevice.ownDeviceId );

		portData.sendPackages.push_back( sendPackage );
	}

	void RuntimeHigh::processPackage( PortData& portData, const Package& package )
	{
		switch( package.baseHeader.messageType )
		{
		case MessageType_ErrorResponse:
			break;

		case MessageType_PingRequest:
			break;

		case MessageType_PingResponse:
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
		PendingPackage& package = portData.pendingPackages[ deviceId ];
		while( !reader.isEnd() )
		{
			switch( package.state )
			{
			case PackageState_WaitForMagic:
				readMagic( package, reader );
				break;

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
				readStore( pPort, portData, package, deviceId );
				break;
			}
		}
	}

	void RuntimeHigh::readMagic( PendingPackage& package, Reader& reader )
	{
		if( package.data.waitForMagic.firstReadCounter < sizeof( s_messageBaseHeaderMagic ) )
		{
			package.data.waitForMagic.firstReadCounter += reader.readShort( package.data.waitForMagic.lastMagic, package.data.waitForMagic.firstReadCounter );

			if( package.data.waitForMagic.firstReadCounter == sizeof( s_messageBaseHeaderMagic ) &&
				package.data.waitForMagic.lastMagic == s_messageBaseHeaderMagic )
			{
				setState( package, PackageState_ReadBaseHeader );
				return;
			}
		}

		uint8 nextByte;
		while( reader.readByte( nextByte ) )
		{
			package.data.waitForMagic.lastMagic <<= 8u;
			package.data.waitForMagic.lastMagic |= nextByte;

			if( package.data.waitForMagic.lastMagic == s_messageBaseHeaderMagic )
			{
				setState( package, PackageState_ReadBaseHeader );
				return;
			}
		}
	}

	void RuntimeHigh::readBaseHeader( PendingPackage& package, Reader& reader )
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

	void RuntimeHigh::readBytes( std::vector<uint8>& target, PendingPackage& package, Reader& reader, PackageState nextState )
	{
		package.data.readBytes.alreadyRead += reader.readData( target.data(), (uintreg)target.size(), package.data.readBytes.alreadyRead );
		if( package.data.readBytes.alreadyRead == target.size() )
		{
			setState( package, nextState );
		}
	}

	void RuntimeHigh::readStore( Port* pPort, PortData& portData, PendingPackage& package, DeviceId deviceId )
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

		setState( package, PackageState_WaitForMagic );
	}

	void RuntimeHigh::setState( PendingPackage& package, PackageState state )
	{
		package.state = state;

		switch( state )
		{
		case PackageState_WaitForMagic:
			package.data.waitForMagic.firstReadCounter = 0u;
			package.data.waitForMagic.lastMagic = 0u;
			break;

		case PackageState_ReadBaseHeader:
		case PackageState_ReadSourceAddress:
		case PackageState_ReadDestinationAddress:
		case PackageState_ReadPayload:
			package.data.readBytes.alreadyRead = 0u;
			break;
		}
	}
}