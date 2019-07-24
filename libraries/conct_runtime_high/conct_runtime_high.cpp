#include "conct_runtime_high.h"

#include "conct_buffered_value_builder.h"
#include "conct_command.h"
#include "conct_crc16.h"
#include "conct_device.h"
#include "conct_filesystem.h"
#include "conct_path.h"
#include "conct_port.h"
#include "conct_proxy.h"
#include "conct_reader.h"
#include "conct_runtime.h"
#include "conct_trace.h"
#include "conct_value_high.h"
#include "conct_writer.h"

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
#	include "conct_crypto_random.h"

#	include <Curve25519.h>
#endif

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
			m_devices.remove( kvp.value );
		}

		m_ports.remove( pPort );
	}

	void RuntimeHigh::processPort( Port* pPort )
	{
		PortData& portData = m_ports[ pPort ];

		uintreg endpointId;
		while( pPort->popConnectionReset( endpointId ) )
		{
			setState( portData.pendingPackages[ endpointId ], PackageState_ReadBaseHeader );

			const DeviceId& endpointDeviceId = portData.endpointToDevice[ endpointId ];
			DeviceData& deviceData = m_devices[ endpointDeviceId ];

			for( DeviceData::CommandMap::PairType& pair : deviceData.commands )
			{
				m_finishCommands.pushBack( pair.value );
				pair.value->setResponse( ResultId_ConnectionLost );
			}

			m_devices.remove( endpointDeviceId );
			portData.endpointToDevice.remove( endpointId );
		}

		ArrayView< uintreg > endpoints;
		pPort->getEndpoints( endpoints );
		for( uintreg i = 0u; i < endpoints.getLength(); ++i )
		{
			const DeviceId* pEndpointDeviceId = portData.endpointToDevice.find( endpoints[ i ] );
			if( pEndpointDeviceId != nullptr )
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

	void RuntimeHigh::getDevices( Vector< DeviceConnection >& devices ) const
	{
		devices.reserve( m_devices.getLength() );
		for( const DeviceMap::PairType& kvp : m_devices )
		{
			DeviceConnection connection;
			connection.id		= kvp.key;
			connection.status	= kvp.value.status;
			devices.pushBack( connection );
		}
	}

	bool RuntimeHigh::isThisDevice( const DeviceAddress& address ) const
	{
		if( address.address[ 0u ] == InvalidDeviceId )
		{
			return false;
		}

		const DeviceData* pDevice = m_devices.find( address.address[ 0u ] );
		if( pDevice == nullptr )
		{
			return false;
		}

		if( pDevice->ownDeviceId == InvalidDeviceId ||
			address.address[ 1u ] == InvalidDeviceId )
		{
			return false;
		}

		return pDevice->ownDeviceId == address.address[ 1u ];
	}

	void RuntimeHigh::changeDevice( DeviceId id, DeviceStatus status )
	{
		CONCT_BREAK;
	}

	CommandId RuntimeHigh::getNextCommandId( DeviceId deviceId )
	{
		DeviceData* pDevice = m_devices.find( deviceId );
		if( pDevice == nullptr )
		{
			return InvalidCommandId;
		}

		const CommandId commandId = pDevice->nextCommandId;
		while( ++pDevice->nextCommandId == InvalidCommandId );

		return commandId;
	}

	ResultId RuntimeHigh::sendCommandPackage( Command* pCommand, const DeviceAddress& deviceAddress, const ArrayView< uint8 >& payload, MessageType messageType )
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
		for( ReceivedPackage& package : portData.receivedPackages )
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

		DeviceData* pTargetDeviceData = m_devices.find( nextDeviceId );
		if( pTargetDeviceData == nullptr )
		{
			sendErrorResponse( sourcePackage, MessageType_ErrorResponse, ResultId_NoSuchDevice );
			return;
		}

		MessageBaseHeader baseHeader = sourcePackage.baseHeader;
		baseHeader.destinationHops--;
		baseHeader.sourceHops++;

		SendPackage package;
		package.targetEndpointId	= pTargetDeviceData->endpointId;
		package.currentOffset		= 0u;

		const uintreg headerSize = sizeof( baseHeader ) + baseHeader.sourceHops + baseHeader.destinationHops;
		uintreg packageSize = headerSize + sourcePackage.payload.getLength();

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		MessageCryptoHeader cryptoHeader;
		if( pTargetDeviceData->status != DeviceStatus_AwaitCryptoKey )
		{
			crypto::calculateRandomBytes( cryptoHeader.cryptoCounter.data, sizeof( cryptoHeader.cryptoCounter.data ) );
			crypto::calculateRandomBytes( cryptoHeader.cryptoIV.data, sizeof( cryptoHeader.cryptoIV.data ) );

			m_crypto.setKey( pTargetDeviceData->cryptoKey.data, sizeof( pTargetDeviceData->cryptoKey.data ) );
			m_crypto.setIV( cryptoHeader.cryptoIV.data, sizeof( cryptoHeader.cryptoIV.data ) );
			m_crypto.setCounter( cryptoHeader.cryptoCounter.data, sizeof( cryptoHeader.cryptoCounter.data ) );

			packageSize += sizeof( cryptoHeader );
		}
#endif

		package.data.reserve( packageSize );

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		if( pTargetDeviceData->status != DeviceStatus_AwaitCryptoKey )
		{
			const uint8* pCryptoHeaderData = (const uint8*)&cryptoHeader;
			package.data.pushRange( pCryptoHeaderData, sizeof( cryptoHeader ) );
		}
#endif

		const uint8* pHeaderData = ( const uint8* )&baseHeader;
		package.data.pushRange( pHeaderData, sizeof( baseHeader ) );

		package.data.pushBack( pTargetDeviceData->ownDeviceId );
		package.data.pushBack( sourcePackage.deviceId );
		package.data.pushRange( sourcePackage.sourceAddress.getData() + 1u, sourcePackage.sourceAddress.getLength() - 1u );
		package.data.pushRange( sourcePackage.destinationAddress.getData() + 1u, sourcePackage.destinationAddress.getLength() - 1u );
		package.data.pushRange( sourcePackage.payload );

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		if( pTargetDeviceData->status != DeviceStatus_AwaitCryptoKey )
		{
			uint8* pPayload = package.data.getData() + sizeof( cryptoHeader );
			const uintreg payloadSize = packageSize - sizeof( cryptoHeader );
			m_crypto.encrypt( pPayload, pPayload, payloadSize );
		}
#endif

		PortData& targetPortData = m_ports[ pTargetDeviceData->pTargetPort ];
		targetPortData.sendPackages.pushBack( package );
	}

	void RuntimeHigh::processPackage( PortData& portData, ReceivedPackage& package )
	{
		Command* pCommand = nullptr;
		if( package.baseHeader.commandId != InvalidCommandId &&
			( package.baseHeader.messageType == MessageType_GetPropertyResponse ||
			package.baseHeader.messageType == MessageType_SetPropertyResponse ||
			package.baseHeader.messageType == MessageType_CallFunctionResponse ) )
		{
			DeviceData& deviceData = m_devices[ package.deviceId ];
			pCommand = deviceData.commands[ package.baseHeader.commandId ];
			deviceData.commands.remove( package.baseHeader.commandId );

			if( pCommand == nullptr )
			{
				// error
				return;
			}

			m_finishCommands.pushBack( pCommand );
		}

		if( package.baseHeader.messageResult != ResultId_Success )
		{
			if( pCommand != nullptr )
			{
				pCommand->setResponse( package.baseHeader.messageResult );
			}
			return;
		}

		switch( package.baseHeader.messageType )
		{
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
						sendErrorResponse( package, MessageType_GetPropertyResponse, ResultId_NoSuchInstance );
						return;
					}

					if( !pInstance->pProxy->getProperty( valueBuilder, pInstance->pInstance, request.nameCrc ) )
					{
						sendErrorResponse( package, MessageType_GetPropertyResponse, ResultId_NoSuchField );
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
					sendErrorResponse( package, MessageType_SetPropertyResponse, ResultId_NoSuchInstance );
					return;
				}

				if( !pInstance->pProxy->setProperty( pInstance->pInstance, request.nameCrc, request.value ) )
				{
					sendErrorResponse( package, MessageType_SetPropertyResponse, ResultId_NoSuchField );
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
						sendErrorResponse( package, MessageType_CallFunctionRequest, ResultId_NoSuchInstance );
						return;
					}

					if( !pInstance->pProxy->callFunction( valueBuilder, pInstance->pInstance, request.nameCrc, request.arguments.toView() ) )
					{
						sendErrorResponse( package, MessageType_CallFunctionRequest, ResultId_NoSuchField );
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

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		case MessageType_CryptoHandshake:
			{
				DeviceData* pDevice = m_devices.find( package.deviceId );
				if( pDevice == nullptr )
				{
					trace::write( "Received package from unknown Device." );
					return;
				}

				if( pDevice->status != DeviceStatus_AwaitCryptoKey )
				{
					trace::write( "Receive duplicate 'CryptoHandshake'" );
					return;
				}

				CryptoHandshake cryptoHandshake = *(const CryptoHandshake*)package.payload.getData();
				if( !Curve25519::dh2( cryptoHandshake.publicKey.data, pDevice->cryptoKey.data ) )
				{
					trace::write( "Crypto handshake failed!" );
					sendErrorResponse( package, MessageType_CryptoHandshake, ResultId_KeyExchangeFailed );
				}

				pDevice->cryptoKey	= cryptoHandshake.publicKey;
				pDevice->status		= DeviceStatus_Unknown;
			}
			break;
#endif

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

		const DeviceId deviceId = m_nextDeviceId;
		m_nextDeviceId++;

		DeviceData& deviceData = m_devices[ deviceId ];
		deviceData.nextCommandId	= FirstCommandId;
		deviceData.endpointId		= endpointId;
		deviceData.ownDeviceId		= ownDeviceId;
		deviceData.pTargetPort		= pPort;
		deviceData.status			= DeviceStatus_Unknown;

		portData.endpointToDevice[ endpointId ] = deviceId;

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		{
			deviceData.status = DeviceStatus_AwaitCryptoKey;

			CryptoHandshake cryptoHandshake;
			Curve25519::dh1( cryptoHandshake.publicKey.data, deviceData.cryptoKey.data );

			DeviceAddress deviceAddress;
			deviceAddress.address[ 0u ] = deviceId;
			deviceAddress.address[ 1u ] = InvalidDeviceId;

			const ArrayView< uint8 > payload( (const uint8*)&cryptoHandshake, sizeof( cryptoHandshake ) );
			sendPackage( deviceAddress, payload, InvalidCommandId, MessageType_CryptoHandshake, ResultId_Success );
		}
#endif

		return deviceId;
	}

	RuntimeHigh::DeviceData* RuntimeHigh::findDevice( PortData& portData, uintreg endpointId )
	{
		const DeviceId* pDeviceId = portData.endpointToDevice.find( endpointId );
		if( pDeviceId == nullptr )
		{
			return nullptr;
		}

		return m_devices.find( *pDeviceId );
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
#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		const DeviceData* pDevice = findDevice( portData, endpointId );
		const bool encrypted = (pDevice != nullptr && pDevice->status != DeviceStatus_AwaitCryptoKey);
#else
		const bool encrypted = false;
#endif

		PendingReceivedPackage& package = portData.pendingPackages[ endpointId ];
		while( !reader.isEnd() )
		{
#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
			if( package.state == PackageState_ReadCryptoHeader )
			{
				readCryptoHeader( package, reader, pDevice );
			}
#endif

			if( package.state == PackageState_ReadBaseHeader )
			{
				readBaseHeader( package, reader, encrypted );
			}

			if( package.state == PackageState_ReadSourceAddress )
			{
				readBytes( package.target.sourceAddress, package, reader, PackageState_ReadDestinationAddress, encrypted );
			}

			if( package.state == PackageState_ReadDestinationAddress )
			{
				readBytes( package.target.destinationAddress, package, reader, PackageState_ReadPayload, encrypted );
			}

			if( package.state == PackageState_ReadPayload )
			{
				readBytes( package.target.payload, package, reader, PackageState_PushToQueue, encrypted );
			}

			if( package.state == PackageState_PushToQueue )
			{
				readStore( pPort, portData, package, endpointId );
			}
		}
	}

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
	void RuntimeHigh::readCryptoHeader( PendingReceivedPackage& package, Reader& reader, const DeviceData* pDevice )
	{
		if( pDevice != nullptr && pDevice->status != DeviceStatus_AwaitCryptoKey )
		{
			package.data.readBytes.alreadyRead += reader.readStruct( package.target.cryptoHeader, package.data.readBytes.alreadyRead );
			if( package.data.readBytes.alreadyRead < sizeof( package.target.cryptoHeader ) )
			{
				return;
			}

			m_crypto.setKey( pDevice->cryptoKey.data, sizeof( pDevice->cryptoKey ) );
			m_crypto.setIV( package.target.cryptoHeader.cryptoIV.data, sizeof( package.target.cryptoHeader.cryptoIV ) );
			m_crypto.setCounter( package.target.cryptoHeader.cryptoCounter.data, sizeof( package.target.cryptoHeader.cryptoCounter ) );
		}

		setState( package, PackageState_ReadBaseHeader );
	}
#endif

	void RuntimeHigh::readBaseHeader( PendingReceivedPackage& package, Reader& reader, bool encrypted )
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

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		if( encrypted )
		{
			uint8* pBaseHeader = (uint8*)&package.target.baseHeader;
			m_crypto.decrypt( pBaseHeader, pBaseHeader, sizeof( package.target.baseHeader ) );
		}
#endif

		package.target.sourceAddress.setLength( package.target.baseHeader.sourceHops );
		package.target.destinationAddress.setLength( package.target.baseHeader.destinationHops );
		package.target.payload.setLength( package.target.baseHeader.payloadSize );

		setState( package, PackageState_ReadSourceAddress );
	}

	void RuntimeHigh::readBytes( Vector<uint8>& target, PendingReceivedPackage& package, Reader& reader, PackageState nextState, bool encrypted )
	{
		package.data.readBytes.alreadyRead += reader.readData( target.getData(), target.getLength(), package.data.readBytes.alreadyRead );
		if( package.data.readBytes.alreadyRead == target.getLength() )
		{
#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
			if( encrypted )
			{
				m_crypto.decrypt( target.getData(), target.getData(), target.getLength() );
			}
#endif

			setState( package, nextState );
		}
	}

	void RuntimeHigh::readStore( Port* pPort, PortData& portData, PendingReceivedPackage& package, uintreg endpointId )
	{
		const DeviceId ownDeviceId = package.target.destinationAddress.getFront();

		const DeviceId* pEndpointDeviceId = portData.endpointToDevice.find( endpointId );
		if( pEndpointDeviceId != nullptr )
		{
			DeviceData& deviceData = m_devices[ *pEndpointDeviceId ];

			CONCT_ASSERT( deviceData.ownDeviceId == InvalidDeviceId || deviceData.ownDeviceId == ownDeviceId );
			deviceData.ownDeviceId = ownDeviceId;

			package.target.deviceId = *pEndpointDeviceId;
		}
		else
		{
			const DeviceId deviceId = addDevice( pPort, portData, ownDeviceId, endpointId );
			package.target.deviceId = deviceId;
		}

		portData.receivedPackages.pushBack( package.target );

		setState( package, PackageState_First );
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
#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		case PackageState_ReadCryptoHeader:
#endif
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

		DeviceData* pDevice = m_devices.find( deviceAddress.address[ 0u ] );
		if( pDevice == nullptr )
		{
			return ResultId_NoSuchDevice;
		}

		baseHeader.sourceHops		= 1u;
		baseHeader.payloadSize		= uint16( payload.getLength() );
		baseHeader.commandId		= commandId;
		baseHeader.messageType		= messageType;
		baseHeader.messageResult	= result;

		const uintreg headerSize = sizeof( baseHeader ) + baseHeader.sourceHops + baseHeader.destinationHops;
		uintreg packageSize = headerSize + payload.getLength();

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		MessageCryptoHeader cryptoHeader;
		if( pDevice->status != DeviceStatus_AwaitCryptoKey )
		{
			crypto::calculateRandomBytes( cryptoHeader.cryptoCounter.data, sizeof( cryptoHeader.cryptoCounter.data ) );
			crypto::calculateRandomBytes( cryptoHeader.cryptoIV.data, sizeof( cryptoHeader.cryptoIV.data ) );

			m_crypto.setKey( pDevice->cryptoKey.data, sizeof( pDevice->cryptoKey.data ) );
			m_crypto.setIV( cryptoHeader.cryptoIV.data, sizeof( cryptoHeader.cryptoIV.data ) );
			m_crypto.setCounter( cryptoHeader.cryptoCounter.data, sizeof( cryptoHeader.cryptoCounter.data ) );

			packageSize += sizeof( cryptoHeader );
		}
#endif

		SendPackage package;
		package.targetEndpointId	= pDevice->endpointId;
		package.currentOffset		= 0u;

		package.data.reserve( packageSize );

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		if( pDevice->status != DeviceStatus_AwaitCryptoKey )
		{
			const uint8* pCryptoHeaderData = (const uint8*)&cryptoHeader;
			package.data.pushRange( pCryptoHeaderData, sizeof( cryptoHeader ) );
		}
#endif

		const uint8* pBaseHeaderData = ( const uint8* )&baseHeader;
		package.data.pushRange( pBaseHeaderData, sizeof( baseHeader ) );

		package.data.pushBack( pDevice->ownDeviceId );

		for( uintreg i = 0u; i < baseHeader.destinationHops; ++i )
		{
			package.data.pushBack( deviceAddress.address[ i ] );
		}

		package.data.pushRange( payload );

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		if( pDevice->status != DeviceStatus_AwaitCryptoKey )
		{
			uint8* pPayload = package.data.getData() + sizeof( cryptoHeader );
			const uintreg payloadSize = packageSize - sizeof( cryptoHeader );
			m_crypto.encrypt( pPayload, pPayload, payloadSize );
		}
#endif

		PortData& portData = m_ports[ pDevice->pTargetPort ];
		portData.sendPackages.pushBack( package );

		return ResultId_Success;
	}

	ResultId RuntimeHigh::sendResponse( const ReceivedPackage& package, const ArrayView< uint8 >& payload, MessageType messageType )
	{
		DeviceAddress address;
		getDeviceAddress( address, package.deviceId, package.sourceAddress );

		return sendPackage( address, payload, package.baseHeader.commandId, messageType, ResultId_Success );
	}

	ResultId RuntimeHigh::sendErrorResponse( const ReceivedPackage& package, MessageType messageType, ResultId result )
	{
		DeviceAddress address;
		getDeviceAddress( address, package.deviceId, package.sourceAddress );

		return sendPackage( address, ArrayView< uint8 >(), package.baseHeader.commandId, messageType, result );
	}
}