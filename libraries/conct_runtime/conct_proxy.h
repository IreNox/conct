#pragma once

#include "conct_array_view.h"
#include "conct_value_builder.h"

namespace conct
{
	class Proxy
	{
	public:

		TypeCrc			getTypeCrc() const { return m_typeCrc; }

		virtual bool	getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, uint16 nameCrc ) const = 0;
		virtual bool	setProperty( void* pInstance, uint16 nameCrc, const Value& value ) const = 0;

		virtual bool	callFunction( ValueBuilder& targetValueBuilder, void* pInstance, uint16 nameCrc, const ArrayView< Value >& parameters ) const = 0;

	protected:

						Proxy( TypeCrc typeCrc );

	private:

		TypeCrc			m_typeCrc;
	};
}