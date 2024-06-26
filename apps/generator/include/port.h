#pragma once

#include "capabilities.h"

namespace conct
{
	class PortCollection;
	class PortParameter;

	class Port
	{
		friend class PortCollection;

	public:

		using ParameterMap = HashMap< DynamicString, PortParameter* >;

		const DynamicString&	getName() const { return m_name;  }

		const DynamicString&	getLibrary() const { return m_library; }
		const DynamicString&	getClass() const { return m_class; }
		const DynamicString&	getParametersStruct() const { return m_parametersStruct; }

		const ParameterMap&		getParameters() const { return m_parameters; }

		size_t					getMtu() const { return m_mtu; }
		bool					getSingleConnection() const { return m_singleConnection; }
		bool					getMultiConnection() const { return m_multiConnection; }
		bool					getReliable() const { return m_reliable; }

		const Capabilities&		getRequirements() const { return m_requirements; }

	private: // friend

								Port();
								~Port();

		bool					load( const Path& filename );

	private:

		DynamicString			m_name;

		DynamicString			m_library;
		DynamicString			m_class;
		DynamicString			m_parametersStruct;

		ParameterMap			m_parameters;

		size_t					m_mtu;
		bool					m_singleConnection;
		bool					m_multiConnection;
		bool					m_reliable;

		Capabilities			m_requirements;
	};
}