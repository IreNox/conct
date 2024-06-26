#pragma once

#include "conct_type.h"

#include <tiki/tiki_dynamic_string.h>

namespace conct
{
	struct StructField
	{
		DynamicString	name;
		const Type*		pType;
	};

	class StructType : public Type
	{
		friend class TypeCollection;

	public:

		typedef DynamicArray< StructField > FieldVector;

		const FieldVector&		getFields() const { return m_fields; }

	private: // friend

								StructType();
		void					create( const Path& fileName, const DynamicString& namespaceVar, const DynamicString& name );
		bool					load( TypeCollection& typeCollection );

	private:

		FieldVector				m_fields;
	};
}