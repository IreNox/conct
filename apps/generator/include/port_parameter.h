#pragma once

#include "conct_dynamic_string.h"
#include "conct_vector.h"

#include "hardware.h"

namespace tinyxml2
{
	class XMLElement;
}

namespace conct
{
	enum PortParameterValueType
	{
		PortParameterValueType_Invalid,

		PortParameterValueType_String,
		PortParameterValueType_Integer
	};

	struct PortParameterValue
	{
		PortParameterValueType	type;
		DynamicString			string;
		int						integer;
	};

	enum PortParameterFilterType
	{
		PortParameterFilterType_HardwareRuntime
	};

	struct PortParameterFilter
	{
		PortParameterFilterType	type;
		HardwareRuntime			runtime;
	};

	class PortParameter
	{
		friend class Port;

	public:

		typedef Vector< PortParameterFilter > FilterVector;
		typedef Vector< PortParameterValue > ValueVector;

		const DynamicString&		getName() const { return m_name;  }
		PortParameterValueType		getType() const { return m_type; }

		const FilterVector&			getFilters() const { return m_filters; }

		const PortParameterValue&	getMinValue() const { return m_minValue; }
		const PortParameterValue&	getMaxValue() const { return m_maxValue; }
		const PortParameterValue&	getDefaultValue() const { return m_defaultValue; }
		const ValueVector&			getValues() const { return m_values; }

		bool						getRequired() const { return m_required; }

	private: // friend

								PortParameter();

		bool					load( tinyxml2::XMLElement* pParameterNode );

	private:

		DynamicString			m_name;
		PortParameterValueType	m_type;

		FilterVector			m_filters;

		PortParameterValue		m_minValue;
		PortParameterValue		m_maxValue;
		PortParameterValue		m_defaultValue;
		ValueVector				m_values;

		bool					m_required;

		bool					loadValue( PortParameterValue& value, const char* pValueText ) const;
		bool					isValueEquals( const PortParameterValue& lhs, const PortParameterValue& rhs ) const;
	};
}