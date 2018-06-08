#pragma once

#include "conct_array_view.h"
#include "conct_value_builder.h"

namespace conct
{
	class Proxy
	{
	public:

		TypeCrc			getTypeCrc() const { return m_typeCrc; }

		virtual bool	getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, const char* pName ) const = 0;
		virtual bool	setProperty( void* pInstance, const char* pName, const Value& value ) const = 0;

		virtual bool	callFunction( ValueBuilder& targetValueBuilder, void* pInstance, const char* pName, const ArrayView< Value >& parameters ) const = 0;

	protected:

						Proxy( TypeCrc typeCrc );

	private:

		TypeCrc			m_typeCrc;
	};
}