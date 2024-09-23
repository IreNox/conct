#pragma once

#include <tiki/tiki_array_view.h>

namespace conct
{
	class Value;

	class Proxy
	{
	public:

		TypeCrc			getTypeCrc() const { return m_typeCrc; }

		virtual bool	getProperty( Value& targetValue, const void* pInstance, uint16 nameCrc ) const = 0;
		virtual bool	setProperty( void* pInstance, uint16 nameCrc, const Value& value ) const = 0;

		virtual bool	callFunction( Value& targetValue, void* pInstance, uint16 nameCrc, const ArrayView< const Value >& parameters ) const = 0;

	protected:

						Proxy( TypeCrc typeCrc );

	private:

		TypeCrc			m_typeCrc;
	};
}