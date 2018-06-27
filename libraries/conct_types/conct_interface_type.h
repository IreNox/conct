#pragma once

#include "conct_path.h"
#include "conct_type.h"
#include "conct_vector.h"

namespace conct
{
	struct InterfaceProperty
	{
		DynamicString	name;
		const Type*		pType;

		bool			hasGetter;
		bool			hasSetter;
	};

	struct InterfaceFunctionParameter
	{
		DynamicString	name;
		const Type*		pType;
	};

	struct InterfaceFunction
	{
		typedef Vector< InterfaceFunctionParameter > ParameterVector;

		DynamicString	name;
		const Type*		pReturnType;

		ParameterVector	parameters;
	};

	struct InterfaceEvent
	{
		DynamicString	name;
		const Type*		pType;
	};

	class InterfaceType : public Type
	{
		friend class TypeCollection;

	public:

		typedef Vector< InterfaceProperty > PropertyVector;
		typedef Vector< InterfaceFunction > FunctionVector;
		typedef Vector< InterfaceEvent > EventVector;

		const Path&				getFileName() const { return m_fileName; }

		const InterfaceType*	getBaseType() const { return m_pBaseType; }

		bool					getInternal() const { return m_internal; }
		bool					getSingleton() const { return m_singleton; }

		const PropertyVector&	getProperties() const { return m_properties; }
		const FunctionVector&	getFunctions() const { return m_functions; }
		const EventVector&		getEvents() const { return m_events; }

	private: // friend

								InterfaceType();
		void					create( const Path& fileName, const DynamicString& namespaceVar, const DynamicString& name );
		bool					load( TypeCollection& typeCollection );

	private:

		Path					m_fileName;

		const InterfaceType*	m_pBaseType;

		bool					m_internal;
		bool					m_singleton;

		PropertyVector			m_properties;
		FunctionVector			m_functions;
		EventVector				m_events;
	};
}