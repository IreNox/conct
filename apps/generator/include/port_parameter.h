#pragma once

#include "hardware.h"

namespace tinyxml2
{
	class XMLElement;
}

namespace conct
{
	class Device;

	enum PortParameterValueType : uint8
	{
		Invalid,

		String,
		Integer
	};

	struct PortParameterValue
	{
		PortParameterValueType	type;
		DynamicString			string;
		int						integer;
	};

	enum class PortParameterFilterType : uint8
	{
		HardwareSystem,
		HardwareEnvironment
	};

	struct PortParameterFilter
	{
		PortParameterFilterType	type;
		HardwareSystem			system;
		HardwareEnvironment		environment;
	};

	class PortParameter
	{
		friend class Port;

	public:

		typedef DynamicArray< PortParameterFilter > FilterVector;
		typedef DynamicArray< PortParameterValue > ValueVector;

		const DynamicString&		getName() const { return m_name;  }
		PortParameterValueType		getType() const { return m_type; }

		const FilterVector&			getFilters() const { return m_filters; }

		const PortParameterValue&	getMinValue() const { return m_minValue; }
		const PortParameterValue&	getMaxValue() const { return m_maxValue; }
		const PortParameterValue&	getDefaultValue() const { return m_defaultValue; }
		const ValueVector&			getValues() const { return m_values; }

		bool						loadValue( PortParameterValue& value, const char* pValueText ) const;
		bool						isValueEquals( const PortParameterValue& lhs, const PortParameterValue& rhs ) const;

		bool						matchFilters( const Device& device ) const;

	private: // friend

									PortParameter();

		bool						load( const tinyxml2::XMLElement* pParameterNode );

	private:

		DynamicString				m_name;
		PortParameterValueType		m_type;

		FilterVector				m_filters;

		PortParameterValue			m_minValue;
		PortParameterValue			m_maxValue;
		PortParameterValue			m_defaultValue;
		ValueVector					m_values;
	};
}