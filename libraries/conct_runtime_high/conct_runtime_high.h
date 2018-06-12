#pragma once

#include "conct_array_view.h"
#include "conct_core.h"
#include "conct_runtime.h"

#include <map>
#include <vector>
#include <queue>

namespace conct
{
	class Device;
	class Port;
	class Reader;
	class CommandBase;

	class RuntimeHigh
	{
	public:

		void					setup( Device* pDevice );

		void					registerPort( Port* pPort );
		void					processPort( Port* pPort );

		CommandId				getNextCommandId( DeviceId deviceId );

		ResultId				sendPackage( CommandBase* pCommand, const DeviceAddress& deviceAddress, const ArrayView< uint8 >& payload, MessageType messageType );

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
			uintreg								alreadyRead;
		};

		union ReceivedPackageStateData
		{
			ReceivedPackageReadBytesData		readBytes;
		};

		struct ReceivedPackage
		{
			DeviceId							deviceId;

			MessageBaseHeader					baseHeader;
			std::vector< DeviceId >				sourceAddress;
			std::vector< DeviceId >				destinationAddress;
			std::vector< uint8 >				payload;
		};

		struct PendingReceivedPackage
		{
			PackageState						state;
			ReceivedPackageStateData			data;
			ReceivedPackage						target;
		};

		struct SendPackage
		{
			uintreg								targetEndpointId;
			std::vector< uint8 >				data;
			uintreg								currentOffset;
		};

		struct PortData
		{
			typedef std::map< uintreg, PendingReceivedPackage > PendingPackageMap;
			typedef std::vector< ReceivedPackage > ReceivedPackageVector;
			typedef std::queue< SendPackage > SendPackageQueue;
			typedef std::map< uintreg, DeviceId > EndpointDeviceMap;

			PendingPackageMap		pendingPackages;
			ReceivedPackageVector	receivedPackages;
			SendPackageQueue		sendPackages;

			EndpointDeviceMap		endpointToDevice;
		};

		struct DeviceData
		{
			typedef std::map< CommandId, CommandBase* > CommandMap;

			Port*		pTargetPort;
			uintreg		endpointId;
			DeviceId	ownDeviceId;
			CommandId	nextCommandId;
			CommandMap	commands;
		};

		typedef std::map< Port*, PortData > PortMap;
		typedef std::map< DeviceId, DeviceData > DeviceMap;

		Device*				m_pDevice;

		PortMap				m_ports;
		DeviceMap			m_devices;

		DeviceId			addDevice( Port* pPort, DeviceId ownDeviceId, uintreg endpointId );

		void				readPort( Port* pPort, PortData& portData );
		void				readPackage( Port* pPort, PortData& portData, Reader& reader, uintreg endpointId );
		//void				readMagic( PendingReceivedPackage& package, Reader& reader );
		void				readBaseHeader( PendingReceivedPackage& package, Reader& reader );
		void				readBytes( std::vector< uint8 >& target, PendingReceivedPackage& package, Reader& reader, PackageState nextState );
		void				readStore( Port* pPort, PortData& portData, PendingReceivedPackage& package, uintreg endpointId );

		void				writePort( Port* pPort, PortData& portData );

		void				processPackages( PortData& portData );
		void				processRoute( PortData& portData, const ReceivedPackage& package );
		void				processPackage( PortData& portData, const ReceivedPackage& package );

		void				setState( PendingReceivedPackage& package, PackageState state );

		void				getDeviceAddress( DeviceAddress& targetAddress, DeviceId targetDeviceId, const std::vector< DeviceId >& sourceAddress ) const;

		ResultId			sendPackage( const DeviceAddress& deviceAddress, const ArrayView< uint8 >& payload, CommandId commandId, MessageType messageType, ResultId result );
		ResultId			sendResponse( const ReceivedPackage& package, const ArrayView< uint8 >& payload, MessageType messageType );
		ResultId			sendErrorResponse( const ReceivedPackage& package, ResultId result );
	};
}
