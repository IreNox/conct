#pragma once

#include "conct_array_view.h"
#include "conct_core.h"
#include "conct_runtime.h"

#include <map>
#include <vector>

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

		void					processPort( Port* pPort );

		CommandId				getNextCommandId( DeviceId deviceId );

		ResultId				sendPackage( CommandBase* pCommand, const DeviceAddress& deviceAddress, const ArrayView< uint8 >& payload );

	private:

		enum PackageState
		{
			PackageState_WaitForMagic,
			PackageState_ReadBaseHeader,
			PackageState_ReadSourceAddress,
			PackageState_ReadDestinationAddress,
			PackageState_ReadPayload,
			PackageState_PushToQueue
		};

		struct PackageWaitForMagicData
		{
			uintreg						firstReadCounter;
			uint16						lastMagic;
		};

		struct PackageReadBytesData
		{
			uintreg						alreadyRead;
		};

		union PackageStateData
		{
			PackageWaitForMagicData		waitForMagic;
			PackageReadBytesData		readBytes;
		};

		struct Package
		{
			MessageBaseHeader		baseHeader;
			std::vector< DeviceId >	sourceAddress;
			std::vector< DeviceId >	destinationAddress;
			std::vector< uint8 >	payload;
		};

		struct PendingPackage
		{
			PackageState			state;
			PackageStateData		data;
			Package					target;
		};

		struct PortData
		{
			typedef std::map< DeviceId, PendingPackage > PendingPackageMap;
			typedef std::vector< Package > PackageVector;

			PendingPackageMap	pendingPackages;
			PackageVector		receivedPackages;
			PackageVector		sendPackages;
		};

		struct DeviceData
		{
			Port*		pTargetPort;
			DeviceId	ownDeviceId;
			CommandId	nextCommandId;
		};

		typedef std::vector< LocalInstance > LocalInstanceVector;
		typedef std::map< Port*, PortData > PortMap;
		typedef std::map< DeviceId, DeviceData > DeviceMap;

		LocalInstanceVector	m_instances;

		PortMap				m_ports;
		DeviceMap			m_devices;

		void				readPort( Port* pPort, PortData& portData );
		void				readPackage( Port* pPort, PortData& portData, Reader& reader, DeviceId deviceId );
		void				readMagic( PendingPackage& package, Reader& reader );
		void				readBaseHeader( PendingPackage& package, Reader& reader );
		void				readBytes( std::vector< uint8 >& target, PendingPackage& package, Reader& reader, PackageState nextState );
		void				readStore( Port* pPort, PortData& portData, PendingPackage& package, DeviceId deviceId );

		void				processPackages( PortData& portData );
		void				processRoute( PortData& portData, const Package& package );
		void				processPackage( PortData& portData, const Package& package );

		void				setState( PendingPackage& package, PackageState state );
	};
}
