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
			//PackageState_WaitForMagic,
			PackageState_ReadBaseHeader,
			PackageState_ReadSourceAddress,
			PackageState_ReadDestinationAddress,
			PackageState_ReadPayload,
			PackageState_PushToQueue
		};

		//struct ReceivedPackageWaitForMagicData
		//{
		//	uintreg								firstReadCounter;
		//	uint16								lastMagic;
		//};

		struct ReceivedPackageReadBytesData
		{
			uintreg								alreadyRead;
		};

		union ReceivedPackageStateData
		{
			//ReceivedPackageWaitForMagicData		waitForMagic;
			ReceivedPackageReadBytesData		readBytes;
		};

		struct ReceivedPackage
		{
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
			DeviceId							targetDeviceId;
			std::vector< uint8 >				data;
			uintreg								currentOffset;
		};

		struct PortData
		{
			typedef std::map< DeviceId, PendingReceivedPackage > PendingPackageMap;
			typedef std::vector< ReceivedPackage > ReceivedPackageVector;
			typedef std::queue< SendPackage > SendPackageQueue;

			PendingPackageMap		pendingPackages;
			ReceivedPackageVector	receivedPackages;
			SendPackageQueue		sendPackages;
		};

		struct DeviceData
		{
			typedef std::map< CommandId, CommandBase* > CommandMap;

			Port*		pTargetPort;
			DeviceId	ownDeviceId;
			CommandId	nextCommandId;
			CommandMap	commands;
		};

		typedef std::vector< LocalInstance > LocalInstanceVector;
		typedef std::map< Port*, PortData > PortMap;
		typedef std::map< DeviceId, DeviceData > DeviceMap;

		LocalInstanceVector	m_instances;

		PortMap				m_ports;
		DeviceMap			m_devices;

		void				readPort( Port* pPort, PortData& portData );
		void				readPackage( Port* pPort, PortData& portData, Reader& reader, DeviceId deviceId );
		//void				readMagic( PendingReceivedPackage& package, Reader& reader );
		void				readBaseHeader( PendingReceivedPackage& package, Reader& reader );
		void				readBytes( std::vector< uint8 >& target, PendingReceivedPackage& package, Reader& reader, PackageState nextState );
		void				readStore( Port* pPort, PortData& portData, PendingReceivedPackage& package, DeviceId deviceId );

		void				writePort( Port* pPort, PortData& portData );

		void				processPackages( PortData& portData );
		void				processRoute( PortData& portData, const ReceivedPackage& package );
		void				processPackage( PortData& portData, const ReceivedPackage& package );

		void				setState( PendingReceivedPackage& package, PackageState state );
	};
}
