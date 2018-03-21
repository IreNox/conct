#pragma once

#include "type.h"

namespace tinyxml2
{
	class XMLDocument;
}

namespace conct
{
	struct InterfaceProperty
	{
		std::string		name;
		const Type*		pType;

		bool			hasGetter;
		bool			hasSetter;
	};

	struct InterfaceFunctionParameter
	{
		std::string		name;
		const Type*		pType;
	};

	struct InterfaceFunction
	{
		typedef std::vector< InterfaceFunctionParameter > ParameterVector;

		std::string		name;
		const Type*		pReturnType;

		ParameterVector	parameters;
	};

	struct InterfaceEvent
	{
		std::string		name;
		const Type*		pType;
	};

	class Interface : public Type
	{
		friend class TypeCollection;

	public:

		typedef std::vector< InterfaceProperty > PropertyVector;
		typedef std::vector< InterfaceFunction > FunctionVector;
		typedef std::vector< InterfaceEvent > EventVector;

		const std::string&		getFileName() const { return m_fileName; }

		const Interface*		getBaseType() const { return m_pBaseType; }

		const PropertyVector&	getProperties() const { return m_properties; }
		const FunctionVector&	getFunctions() const { return m_functions; }
		const EventVector&		getEvents() const { return m_events; }

		//virtual TypeCrc			getCrc() const override final;

	private: // friend

								Interface();
		void					create( const std::string& fileName, const std::string& namespaceVar, const std::string& name );
		bool					load( TypeCollection& typeCollection );

	private:

		std::string				m_fileName;

		const Interface*		m_pBaseType;

		PropertyVector			m_properties;
		FunctionVector			m_functions;
		EventVector				m_events;
	};
}