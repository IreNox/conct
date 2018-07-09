#pragma once

#include "conct_array_view.h"
#include "conct_core.h"
#include "conct_queue.h"
#include "conct_runtime.h"
#include "conct_vector.h"

#include <map>

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
		void					getDevices( Vector< DeviceId >& devices ) const;
		bool					isThisDevice( const DeviceAddress& address ) const;

		CommandId				getNextCommandId( DeviceId deviceId );

		ResultId				sendPackage( Command* pCommand, const DeviceAddress& deviceAddress, const ArrayView< uint8 >& payload, MessageType messageType );
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
			typedef std::map< uintreg, PendingReceivedPackage > PendingPackageMap;
			typedef Vector< ReceivedPackage > ReceivedPackageVector;
			typedef Queue< SendPackage > SendPackageQueue;
			typedef std::map< uintreg, DeviceId > EndpointDeviceMap;

			PendingPackageMap		pendingPackages;
			ReceivedPackageVector	receivedPackages;
			SendPackageQueue		sendPackages;

			EndpointDeviceMap		endpointToDevice;
		};

		struct DeviceData
		{
			typedef std::map< CommandId, Command* > CommandMap;

			Port*		pTargetPort;
			uintreg		endpointId;
			DeviceId	ownDeviceId;
			CommandId	nextCommandId;
			CommandMap	commands;
		};

		typedef std::map< Port*, PortData > PortMap;
		typedef std::map< DeviceId, DeviceData > DeviceMap;
		typedef Queue< Command* > CommandQueue;

		Device*				m_pDevice;

		PortMap				m_ports;
		DeviceMap			m_devices;

		CommandQueue		m_finishCommands;

		DeviceId			addDevice( Port* pPort, DeviceId ownDeviceId, uintreg endpointId );

		void				readPort( Port* pPort, PortData& portData );
		void				readPackage( Port* pPort, PortData& portData, Reader& reader, uintreg endpointId );
		void				readBaseHeader( PendingReceivedPackage& package, Reader& reader );
		void				readBytes( Vector< uint8 >& target, PendingReceivedPackage& package, Reader& reader, PackageState nextState );
		void				readStore( Port* pPort, PortData& portData, PendingReceivedPackage& package, uintreg endpointId );

		void				writePort( Port* pPort, PortData& portData );

		void				processPackages( PortData& portData );
		void				processRoute( PortData& portData, const ReceivedPackage& package );
		void				processPackage( PortData& portData, const ReceivedPackage& package );

		void				setState( PendingReceivedPackage& package, PackageState state );

		void				getDeviceAddress( DeviceAddress& targetAddress, DeviceId targetDeviceId, const Vector< DeviceId >& sourceAddress ) const;

		ResultId			sendPackage( const DeviceAddress& deviceAddress, const ArrayView< uint8 >& payload, CommandId commandId, MessageType messageType, ResultId result );
		ResultId			sendResponse( const ReceivedPackage& package, const ArrayView< uint8 >& payload, MessageType messageType );
		ResultId			sendErrorResponse( const ReceivedPackage& package, ResultId result );
	};
}
