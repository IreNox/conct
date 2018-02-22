#pragma once

namespace conct
{
	template< class T >
	Array< T >::Array( T* pData, uint8_t count )
	{
		m_pData	= pData;
		m_count	= count;
	}

	template< class T >
	T* Array< T >::operator[]( uint8_t index )
	{
		return m_pData[ index ];
	}
}
