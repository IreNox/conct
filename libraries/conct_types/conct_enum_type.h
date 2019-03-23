#pragma once

#include "conct_path.h"
#include "conct_type.h"
#include "conct_vector.h"

namespace conct
{
	struct EnumValue
	{
		DynamicString	name;
		sint64			value;
	};

	class EnumType : public Type
	{
		friend class TypeCollection;

	public:

		typedef Vector< EnumValue > ValueVector;

		const ValueVector&		getValues() const { return m_values; }

	private: // friend

								EnumType();
		void					create( const Path& fileName, const DynamicString& namespaceVar, const DynamicString& name );
		bool					load( TypeCollection& typeCollection );

	private:

		ValueVector				m_values;
	};
}