#pragma once

#include <initializer_list>

namespace conct
{
	template< class T >
	class Vector
	{
	public:

		Vector();
		Vector( const std::initializer_list< T >& initList );

	private:

		std::vector< T >	m_vector;
	};
}
