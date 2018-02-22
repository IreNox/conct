#pragma once

#include "type.h"

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

		const PropertyVector&	getProperties() const { return m_properties; }
		const FunctionVector&	getFunctions() const { return m_functions; }
		const EventVector&		getEvents() const { return m_events; }

	private: // friend

								Interface();
		bool					load( const std::string& fileName, TypeCollection& typeCollection );

	private:

		std::string				m_fileName;

		const Interface*		m_pBase;

		PropertyVector			m_properties;
		FunctionVector			m_functions;
		EventVector				m_events;
	};
}