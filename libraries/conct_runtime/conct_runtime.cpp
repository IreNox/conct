#include "conct_runtime.h"

namespace conct
{
	bool runtime::isDeviceAddressEmpty( const DeviceAddress& address )
	{
		return address.address[ 0u ] == InvalidDeviceId;
	}

	bool runtime::isDeviceAddressEquals( const DeviceAddress& lhs, const DeviceAddress& rhs )
	{
		for( uintreg i = 0u; i < DeviceAddress::Size; ++i )
		{
			if( lhs.address[ i ] != rhs.address[ i ] )
			{
				return false;
			}

			if( lhs.address[ i ] == InvalidDeviceId )
			{
				return true;
			}
		}

		// both addresses are invalid
		return false;
	}

	void runtime::clearDeviceAddress( DeviceAddress& target )
	{
		target.address[ 0u ] = InvalidDeviceId;
	}

	void runtime::copyDeviceAddress( DeviceAddress& target, const DeviceAddress& source )
	{
		for( uintreg i = 0u; i < DeviceAddress::Size; ++i )
		{
			target.address[ i ] = source.address[ i ];

			if( source.address[ i ] == InvalidDeviceId )
			{
				return;
			}
		}
	}

	void runtime::pushToDeviceAddress( DeviceAddress& target, DeviceId deviceId )
	{
		for( uintreg i = 0u; i < DeviceAddress::Size - 1u; ++i )
		{
			if( target.address[ i ] == InvalidDeviceId )
			{
				target.address[ i ] = deviceId;
				target.address[ i + 1u ] = InvalidDeviceId;
				return;
			}
		}
	}

}