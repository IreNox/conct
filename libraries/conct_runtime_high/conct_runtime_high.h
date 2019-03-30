#pragma once

#include "conct_array_view.h"
#include "conct_core.h"
#include "conct_map.h"
#include "conct_queue.h"
#include "conct_runtime.h"
#include "conct_runtime_high_types.h"
#include "conct_vector.h"

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
#	include <ChaCha.h>
#endif

namespace conct
{
	class Device;
	class Port;
	class Reader;
	class Command;

	class RuntimeHigh
	{
	public:

		void					setup( Device* pDevice );

		void					registerPort( Port* pPort );
		void					unregisterPort( Port* pPort );

		void					processPort( Port* pPort );

		uintreg					getDeviceCount() const;
		void					getDevices( Vector< DeviceConnection >& devices ) const;
		bool					isThisDevice( const DeviceAddress& address ) const;
		void					changeDevice( DeviceId id, DeviceStatus status );

		CommandId				getNextCommandId( DeviceId deviceId );

		ResultId				sendCommandPackage( Command* pCommand, const DeviceAddress& deviceAddress, const ArrayView< uint8 >& payload, MessageType messageType );
		bool					popFinishCommand( Command*& pCommand );

	private:

		enum PackageState
		{
			PackageState_ReadBaseHeader,
			PackageState_ReadSourceAddress,
			PackageState_ReadDestinationAddress,
			PackageState_ReadPayload,
			PackageState_PushToQueue
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

			MessageBaseHeader				baseHeader;
			Vector< DeviceId >				sourceAddress;
			Vector< DeviceId >				destinationAddress;
			Vector< uint8 >					payload;
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
			Vector< uint8 >					data;
			uintreg							currentOffset;
		};

		struct PortData
		{
			typedef Map< uintreg, PendingReceivedPackage > PendingPackageMap;
			typedef Vector< ReceivedPackage > ReceivedPackageVector;
			typedef Queue< SendPackage > SendPackageQueue;
			typedef Map< uintreg, DeviceId > EndpointDeviceMap;

			PendingPackageMap		pendingPackages;
			ReceivedPackageVector	receivedPackages;
			SendPackageQueue		sendPackages;

			EndpointDeviceMap		endpointToDevice;
		};

		struct DeviceData
		{
			typedef Map< CommandId, Command* > CommandMap;

			Port*			pTargetPort;
			uintreg			endpointId;
			DeviceId		ownDeviceId;
			DeviceStatus	status;
			CommandId		nextCommandId;
			CommandMap		commands;
#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
			CryptoState		cryptoState;
			CryptoKey		cryptoKey;
			CryptoCounter	cryptoCounter;
#endif
		};

		typedef Map< Port*, PortData > PortMap;
		typedef Map< DeviceId, DeviceData > DeviceMap;
		typedef Queue< Command* > CommandQueue;
		typedef Map< uint32, RuntimeHighStoredDevice > StoredDeviceMap;

		Device*				m_pDevice;

		PortMap				m_ports;
		DeviceMap			m_devices;
		DeviceId			m_nextDeviceId;

		StoredDeviceMap		m_storedDevices;

		CommandQueue		m_finishCommands;

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		ChaCha				m_crypto;

		void				loadDeviceDatabase();
		void				generateAndSendNewKey( DeviceId deviceId, DeviceData* pDevice );
#endif

		DeviceId			addDevice( Port* pPort, PortData& portData, DeviceId ownDeviceId, uintreg endpointId );

		void				readPort( Port* pPort, PortData& portData );
		void				readPackage( Port* pPort, PortData& portData, Reader& reader, uintreg endpointId );
		void				readBaseHeader( PendingReceivedPackage& package, Reader& reader );
		void				readBytes( Vector< uint8 >& target, PendingReceivedPackage& package, Reader& reader, PackageState nextState );
		void				readStore( Port* pPort, PortData& portData, PendingReceivedPackage& package, uintreg endpointId );

		void				writePort( Port* pPort, PortData& portData );

		void				processPackages( PortData& portData );
		void				processRoute( PortData& portData, const ReceivedPackage& package );
		void				processPackage( PortData& portData, ReceivedPackage& package );

		void				setState( PendingReceivedPackage& package, PackageState state );

		void				getDeviceAddress( DeviceAddress& targetAddress, DeviceId targetDeviceId, const Vector< DeviceId >& sourceAddress ) const;

		ResultId			sendPackage( const DeviceAddress& deviceAddress, const ArrayView< uint8 >& payload, CommandId commandId, MessageType messageType, ResultId result );
		ResultId			sendResponse( const ReceivedPackage& package, const ArrayView< uint8 >& payload, MessageType messageType );
		ResultId			sendErrorResponse( const ReceivedPackage& package, MessageType messageType, ResultId result );
	};
}
