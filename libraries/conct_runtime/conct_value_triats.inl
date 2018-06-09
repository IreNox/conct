#pragma once

namespace conct
{
	template<>
	CONCT_FORCE_INLINE TypeCrc ValueTypeTraits< bool >::getTypeCrc()
	{
		return 24858u;
	}

	template<>
	CONCT_FORCE_INLINE TypeCrc ValueTypeTraits< DeviceId >::getTypeCrc()
	{
		return 47711u;
	}

	template<>
	CONCT_FORCE_INLINE TypeCrc ValueTypeTraits< InstanceId >::getTypeCrc()
	{
		return 14236u;
	}

	template< class T >
	CONCT_FORCE_INLINE TypeCrc ValueTypeTraits< T >::getTypeCrc()
	{
		return T::s_typeCrc;
	}
}