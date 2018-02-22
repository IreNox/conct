#pragma  once

#include "type.h"

namespace conct
{
	class Array : public Type
	{
		friend class TypeCollection;

	public:

		const Type*		getBaseType() const { return m_pBaseType; }

	private: // friend

						Array();
		void			create( const std::string& namespaceVar, const std::string& name, const Type* pBaseType );

	private:

		const Type*		m_pBaseType;
	};
}