#pragma once

#include "conct_path.h"
#include "conct_type.h"
#include "conct_vector.h"

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

		typedef Vector< StructField > FieldVector;

		const FieldVector&		getFields() const { return m_fields; }

	private: // friend

								StructType();
		void					create( const Path& fileName, const DynamicString& namespaceVar, const DynamicString& name );
		bool					load( TypeCollection& typeCollection );

	private:

		FieldVector				m_fields;
	};
}