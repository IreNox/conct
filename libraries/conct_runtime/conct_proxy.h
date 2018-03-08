#pragma once

#include "conct_array.h"
#include "conct_value.h"

namespace conct
{
	class Proxy
	{
	public:

		TypeCrc			getTypeCrc() const { return m_typeCrc; }

		virtual bool	getProperty( Value& targetValue, const void* pInstance, const char* pName ) const = 0;
		virtual bool	setProperty( void* pInstance, const char* pName, const Value& value ) = 0;

		virtual bool	callFunction( Value& returnValue, void* pInstance, const char* pName, const Array< Value >& parameters ) = 0;

	protected:

						Proxy( TypeCrc typeCrc );

	private:

		TypeCrc			m_typeCrc;
	};
}