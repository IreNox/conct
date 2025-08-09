#pragma once

#include "conct_core.h"
#include "conct_runtime_types.h"

#include <tiki/tiki_dynamic_array.h>
#include <tiki/tiki_hash_map.h>
#include <tiki/tiki_queue.h>

namespace conct
{
	class Device;
	class Port;
	class BinaryReader;
	class Command;

	class Runtime
	{
	public:

		void					setup( Device* pDevice );

		void					registerPort( Port* pPort );
		void					unregisterPort( Port* pPort );

		void					processPort( Port* pPort );

		uintreg					getDeviceCount() const;
		void					getDevices( DynamicArray< DeviceConnection >& devices ) const;
		bool					isThisDevice( const DeviceAddress& address ) const;
		void					changeDevice( DeviceId id, DeviceStatus status );

		CommandId				getNextCommandId( DeviceId deviceId );

		ResultId				sendCommandPackage( Command* pCommand, const DeviceAddress& deviceAddress, const ArrayView< byte >& payload, MessageType messageType );
		bool					popFinishCommand( Command*& pCommand );

	private:

		enum PackageState
		{
#if TIKI_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
			PackageState_ReadCryptoHeader,
#endif
			PackageState_ReadBaseHeader,
			PackageState_ReadSourceAddress,
			PackageState_ReadDestinationAddress,
			PackageState_ReadPayload,
			PackageState_PushToQueue,

#if TIKI_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
			PackageState_First					= PackageState_ReadCryptoHeader
#else
			PackageState_First					= PackageState_ReadBaseHeader
#endif
		};

		struct ReceivedPackageReadBytesData
		{
			uintreg							alreadyRead;
		};

		union ReceivedPackageStateData
		{
			ReceivedPackageReadBytesData	readBytes;
		};

		struct ReceivedPackage
		{
			DeviceId						deviceId;

#if TIKI_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
			MessageCryptoHeader				cryptoHeader;
#endif
			MessageBaseHeader				baseHeader;
			DynamicArray< DeviceId >		sourceAddress;
			DynamicArray< DeviceId >		destinationAddress;
			DynamicArray< byte >			payload;
		};

		struct PendingReceivedPackage
		{
			PackageState					state;
			ReceivedPackageStateData		data;
			ReceivedPackage					target;
		};

		struct SendPackage
		{
			uintreg							targetEndpointId;
			DynamicArray< byte >			data;
			uintreg							currentOffset;
		};

		struct PortData
		{
			typedef HashMap< uintreg, PendingReceivedPackage > PendingPackageMap;
			typedef DynamicArray< ReceivedPackage > ReceivedPackageVector;
			typedef Queue< SendPackage > SendPackageQueue;
			typedef HashMap< uintreg, DeviceId > EndpointDeviceMap;

			PendingPackageMap		pendingPackages;
			ReceivedPackageVector	receivedPackages;
			SendPackageQueue		sendPackages;

			EndpointDeviceMap		endpointToDevice;
		};

		struct DeviceData
		{
			typedef HashMap< CommandId, Command* > CommandMap;

			Port*			pTargetPort;
			uintreg			endpointId;
			DeviceId		ownDeviceId;
			DeviceStatus	status;
			CommandId		nextCommandId;
			CommandMap		commands;
#if TIKI_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
			CryptoKey		cryptoKey;
#endif
		};

		typedef HashMap< Port*, PortData > PortMap;
		typedef HashMap< DeviceId, DeviceData > DeviceMap;
		typedef Queue< Command* > CommandQueue;
		typedef HashMap< uint32, RuntimeHighStoredDevice > StoredDeviceMap;

		Device*				m_pDevice;

		PortMap				m_ports;
		DeviceMap			m_devices;
		DeviceId			m_nextDeviceId;

		CommandQueue		m_finishCommands;

#if TIKI_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		ChaCha				m_crypto;
#endif

		DeviceId			addDevice( Port* pPort, PortData& portData, DeviceId ownDeviceId, uintreg endpointId );
		DeviceData*			findDevice( PortData& portData, uintreg endpointId );

		void				readPort( Port* pPort, PortData& portData );
		void				readPackage( Port* pPort, PortData& portData, BinaryReader& reader, uintreg endpointId );
#if TIKI_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		void				readCryptoHeader( PendingReceivedPackage& package, BinaryReader& reader, const DeviceData* pDevice );
#endif
		void				readBaseHeader( PendingReceivedPackage& package, BinaryReader& reader, bool encrypted );
		void				readBytes( DynamicArray< uint8 >& target, PendingReceivedPackage& package, BinaryReader& reader, PackageState nextState, bool encrypted );
		void				readStore( Port* pPort, PortData& portData, PendingReceivedPackage& package, uintreg endpointId );

		void				writePort( Port* pPort, PortData& portData );

		void				processPackages( PortData& portData );
		void				processRoute( PortData& portData, const ReceivedPackage& package );
		void				processPackage( PortData& portData, ReceivedPackage& package );

		void				setState( PendingReceivedPackage& package, PackageState state );

		void				getDeviceAddress( DeviceAddress& targetAddress, DeviceId targetDeviceId, const DynamicArray< DeviceId >& sourceAddress ) const;

		ResultId			sendPackage( const DeviceAddress& deviceAddress, const ConstArrayView< byte >& payload, CommandId commandId, MessageType messageType, ResultId result );
		ResultId			sendResponse( const ReceivedPackage& package, const ConstArrayView< byte >& payload, MessageType messageType );
		ResultId			sendErrorResponse( const ReceivedPackage& package, MessageType messageType, ResultId result );
	};
}
