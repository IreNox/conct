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

	void RuntimeHigh::processPackage( PortData& portData, ReceivedPackage& package )
	{
#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		DeviceData* pDevice = m_devices.find( package.deviceId );
		if( pDevice == nullptr )
		{
			trace::write( "Received 'CryptoHandshake' from unknown Device." );
			return;
		}

		if( pDevice->cryptoState == CryptoState_Encrypted )
		{
			m_crypto.setKey( pDevice->cryptoKey.data, sizeof( pDevice->cryptoKey.data ) );
			m_crypto.setCounter( pDevice->cryptoCounter.data, sizeof( pDevice->cryptoCounter.data ) );
			m_crypto.setIV( package.baseHeader.cryptoIV.data, sizeof( package.baseHeader.cryptoIV.data ) );

			m_crypto.decrypt( package.payload.getData(), package.payload.getData(), package.payload.getLength() );
		}
#endif

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
		case MessageType_CryptoHandshakeRequest:
			{
				const CryptoHandshakeRequest& request = *(const CryptoHandshakeRequest*)package.payload.getData();

				CryptoHandshakeResponse cryptoHandshakeResponse;
				cryptoHandshakeResponse.serialNumber	= m_pDevice->getSerialNumber();
				cryptoHandshakeResponse.keyHash			= 0u;

				const RuntimeHighStoredDevice* pStoredDevice = m_storedDevices.find( request.serialNumber );
				if( pStoredDevice == nullptr )
				{
					cryptoHandshakeResponse.keyHash = pStoredDevice->hash;
				}

				const ArrayView< uint8 > payload( (const uint8*)&cryptoHandshakeResponse, sizeof( cryptoHandshakeResponse ) );
				sendResponse( package, payload, MessageType_CryptoHandshakeResponse );

				if( pStoredDevice != nullptr )
				{
					pDevice->cryptoState	= CryptoState_Encrypted;
					pDevice->cryptoKey		= pStoredDevice->key;
					pDevice->cryptoCounter	= pStoredDevice->counter;
				}
			}
			break;

		case MessageType_CryptoHandshakeResponse:
			{
				const CryptoHandshakeResponse& response = *(const CryptoHandshakeResponse*)package.payload.getData();

				const RuntimeHighStoredDevice* pStoredDevice = m_storedDevices.find( response.serialNumber );
				if( pStoredDevice != nullptr &&
					pStoredDevice->hash == response.keyHash )
				{
					pDevice->cryptoState	= CryptoState_Encrypted;
					pDevice->cryptoKey		= pStoredDevice->key;
					pDevice->cryptoCounter	= pStoredDevice->counter;
					return;
				}

				RuntimeHighStoredDevice storedDevice;
				storedDevice.serialNumber = response.serialNumber;
				crypto::calculateRandomBytes( storedDevice.key.data, sizeof( storedDevice.key.data ) );
				crypto::calculateRandomBytes( storedDevice.counter.data, sizeof( storedDevice.counter.data ) );
				storedDevice.hash = calculateCrc16( storedDevice.key.data, sizeof( storedDevice.key ) + sizeof( storedDevice.counter ) );
				m_storedDevices.insert( storedDevice.serialNumber, storedDevice );

				CryptoAssingKeyRequest cryptoAssingKeyRequest;
				cryptoAssingKeyRequest.key		= storedDevice.key;
				cryptoAssingKeyRequest.counter	= storedDevice.counter;

				const ArrayView< uint8 > payload( (const uint8*)&cryptoAssingKeyRequest, sizeof( cryptoAssingKeyRequest ) );
				sendResponse( package, payload, MessageType_CryptoAssignKeyRequest );
			}
			break;

		case MessageType_CryptoAssignKeyRequest:
			{
				const CryptoAssingKeyRequest& request = *(const CryptoAssingKeyRequest*)package.payload.getData();

				if( pDevice->cryptoState == CryptoState_Encrypted )
				{
					sendErrorResponse( package, MessageType_CryptoHandshakeResponse, ResultId_PermissionDenied );
					return;
				}

				CryptoHandshakeResponse cryptoHandshakeResponse;
				cryptoHandshakeResponse.serialNumber	= m_pDevice->getSerialNumber();
				cryptoHandshakeResponse.keyHash			= calculateCrc16( request.key.data, sizeof( request.key ) + sizeof( request.counter ) );

				const ArrayView< uint8 > payload( (const uint8*)&cryptoHandshakeResponse, sizeof( cryptoHandshakeResponse ) );
				sendResponse( package, payload, MessageType_CryptoHandshakeResponse );

				pDevice->cryptoState	= CryptoState_Encrypted;
				pDevice->cryptoKey		= request.key;
				pDevice->cryptoCounter	= request.counter;
			}
			break;
#endif

		default:
			break;
		}
	}

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
	void RuntimeHigh::loadDeviceDatabase()
	{
		const Result< Vector< uint8 > > databaseResult = filesystem::readBinaryFile( Path( "database.bin" ) );
		if( databaseResult.isFailure() )
		{
			trace::write( "Failed to load device database." );
			return;
		}

		const Vector< uint8 > databaseBlob = databaseResult.value;
		const RuntimeHighDeviceDatabase& database = *(const RuntimeHighDeviceDatabase*)databaseBlob.getBegin();
		for( uintreg i = 0u; i < database.deviceCount; ++i )
		{
			const RuntimeHighStoredDevice& device = database.devices[ i ];
			m_storedDevices.insert( device.serialNumber, device );
		}
	}

	void RuntimeHigh::generateAndSendNewKey( DeviceId deviceId, DeviceData* pDevice )
	{

	}
#endif

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
#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		deviceData.cryptoState		= CryptoState_Unencrypted;
#endif

		portData.endpointToDevice[ endpointId ] = deviceId;

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		{
			CryptoHandshakeRequest cryptoHandshakeRequest;
			cryptoHandshakeRequest.serialNumber = m_pDevice->getSerialNumber();

			DeviceAddress deviceAddress;
			deviceAddress.address[ 0u ] = deviceId;
			deviceAddress.address[ 1u ] = InvalidDeviceId;

			const ArrayView< uint8 > payload( (const uint8*)&cryptoHandshakeRequest, sizeof( cryptoHandshakeRequest ) );
			sendPackage( deviceAddress, payload, InvalidCommandId, MessageType_CryptoHandshakeRequest, ResultId_Success );
		}
#endif

		return deviceId;
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

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		if( pDevice->cryptoState == CryptoState_Encrypted )
		{
			crypto::calculateRandomBytes( baseHeader.cryptoIV.data, sizeof( baseHeader.cryptoIV.data ) );

			m_crypto.setKey( pDevice->cryptoKey.data, sizeof( pDevice->cryptoKey.data ) );
			m_crypto.setCounter( pDevice->cryptoCounter.data, sizeof( pDevice->cryptoCounter.data ) );
			m_crypto.setIV( baseHeader.cryptoIV.data, sizeof( baseHeader.cryptoIV.data ) );
		}
#endif

		SendPackage package;
		package.targetEndpointId	= pDevice->endpointId;
		package.currentOffset		= 0u;

		const uintreg headerSize = sizeof( baseHeader ) + baseHeader.sourceHops + baseHeader.destinationHops;
		const uintreg packageSize = headerSize + payload.getLength();
		package.data.reserve( packageSize );

		const uint8* pHeaderData = ( const uint8* )&baseHeader;
		package.data.pushRange( pHeaderData, sizeof( baseHeader ) );

		package.data.pushBack( pDevice->ownDeviceId );

		for( uintreg i = 0u; i < baseHeader.destinationHops; ++i )
		{
			package.data.pushBack( deviceAddress.address[ i ] );
		}

		package.data.pushRange( payload );

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		if( pDevice->cryptoState == CryptoState_Encrypted )
		{
			uint8* pPayload = package.data.getData() + headerSize;
			m_crypto.encrypt( pPayload, pPayload, payload.getLength() );
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