#pragma once

#include "type.h"

namespace conct
{
	struct StructField
	{
		std::string		name;
		const Type*		pType;
	};

	class StructType : public Type
	{
		friend class TypeCollection;

	public:

		typedef std::vector< StructField > FieldVector;

		const std::string&		getFileName() const { return m_fileName; }

		const FieldVector&		getFields() const { return m_fields; }

	private: // friend

								StructType();
		void					create( const std::string& fileName, const std::string& namespaceVar, const std::string& name );
		bool					load( TypeCollection& typeCollection );

	private:

		std::string				m_fileName;

		FieldVector				m_fields;
	};
}