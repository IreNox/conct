#include "port_parameter.h"

#include "conct_trace.h"
#include "conct_xml_helper.h"

#include "device.h"

#include <tinyxml2/tinyxml2.h>

namespace conct
{
	static const Pair< PortParameterFilterType, const char* > s_portParameterFilterTypeMapping[] =
	{
		{ PortParameterFilterType::HardwareSystem,		"system" },
		{ PortParameterFilterType::HardwareEnvironment,	"environment" }
	};
	static const ArrayView< const Pair< PortParameterFilterType, const char* > > s_portParameterFilterTypeMappingView( s_portParameterFilterTypeMapping, TIKI_ARRAY_COUNT( s_portParameterFilterTypeMapping ) );
	TIKI_STATIC_ASSERT( TIKI_ARRAY_COUNT( s_portParameterFilterTypeMapping ) == 2u );

	PortParameter::PortParameter()
	{
		m_type				= PortParameterValueType::Invalid;
		m_minValue.type		= PortParameterValueType::Invalid;
		m_maxValue.type		= PortParameterValueType::Invalid;
		m_defaultValue.type	= PortParameterValueType::Invalid;
	}

	bool PortParameter::load( const tinyxml2::XMLElement* pParameterNode )
	{
		DynamicString typeString;
		if( !loadStringValue( m_name, pParameterNode, "name" ) ||
			!loadStringValue( typeString, pParameterNode, "type" ) )
		{
			return false;
		}

		if( typeString == "string" )
		{
			m_type = PortParameterValueType::String;
		}
		else if( typeString == "integer" )
		{
			m_type = PortParameterValueType::Integer;
		}
		else
		{
			trace::write( "Error: Type '"_s + typeString + "' in parameter '" + m_name + "' is invalid.\n" );
			return false;
		}

		const tinyxml2::XMLElement* pFiltersNode = pParameterNode->FirstChildElement( "filters" );
		if( pFiltersNode != nullptr )
		{
			for( const tinyxml2::XMLElement* pFilterNode = pFiltersNode->FirstChildElement( "filter" ); pFilterNode != nullptr; pFilterNode = pFilterNode->NextSiblingElement( "filter" ) )
			{
				PortParameterFilter filter;
				if( !loadEnumValue( filter.type, pFilterNode, "type", s_portParameterFilterTypeMappingView ) )
				{
					return false;
				}

				switch( filter.type )
				{
				case PortParameterFilterType::HardwareSystem:
					if( !loadEnumValue( filter.system, pFilterNode, "value", getHardwareSystemMapping() ) )
					{
						return false;
					}
					break;

				case PortParameterFilterType::HardwareEnvironment:
					if( !loadEnumValue( filter.environment, pFilterNode, "value", getHardwareEnvironmentMapping() ) )
					{
						return false;
					}
					break;
				}

				m_filters.pushBack( filter );
			}
		}

		DynamicString valueString;
		if( loadStringValue( valueString, pParameterNode, "min", true ) )
		{
			if( m_type != PortParameterValueType::Integer )
			{
				traceNodeError( pParameterNode, "Error: min attribute is only valid for numeric types.\n" );
				return false;
			}

			if( !loadValue( m_minValue, valueString.toConstCharPointer() ) )
			{
				traceNodeError( pParameterNode, "Error: min attribute: '"_s + valueString + "' is not a valid value for this parameter.\n" );
				return false;
			}
		}

		if( loadStringValue( valueString, pParameterNode, "max", true ) )
		{
			if( m_type != PortParameterValueType::Integer )
			{
				traceNodeError( pParameterNode, "Error: max attribute is only valid for numeric types.\n" );
				return false;
			}

			if( !loadValue( m_maxValue, valueString.toConstCharPointer() ) )
			{
				traceNodeError( pParameterNode, "Error: max attribute: '"_s + valueString + "' is not a valid value for this parameter.\n" );
				return false;
			}
		}

		if( loadStringValue( valueString, pParameterNode, "default", true ) )
		{
			if( !loadValue( m_defaultValue, valueString.toConstCharPointer() ) )
			{
				traceNodeError( pParameterNode, "Error: default attribute: '"_s + valueString + "' is not a valid value for this parameter.\n" );
				return false;
			}

			if( m_minValue.type != PortParameterValueType::Invalid &&
				m_minValue.integer > m_defaultValue.integer )
			{
				traceNodeError( pParameterNode, "Error: default attribute: '"_s + valueString + "' is lower than min value of '" + string_tools::toString( m_minValue.integer ) + "'.\n" );
				return false;
			}

			if( m_maxValue.type != PortParameterValueType::Invalid &&
				m_maxValue.integer < m_defaultValue.integer )
			{
				traceNodeError( pParameterNode, "Error: default attribute: '"_s + valueString + "' is greater than max value of '" + string_tools::toString( m_maxValue.integer ) + "'.\n" );
				return false;
			}
		}

		const tinyxml2::XMLElement* pValuesNode = pParameterNode->FirstChildElement( "values" );
		if( pValuesNode != nullptr )
		{
			if( m_minValue.type != PortParameterValueType::Invalid ||
				m_maxValue.type != PortParameterValueType::Invalid )
			{
				traceNodeError( pValuesNode, "Error: parameter can't have min/max and values.\n" );
				return false;
			}

			for( const tinyxml2::XMLElement* pValueNode = pValuesNode->FirstChildElement( "value" ); pValueNode != nullptr; pValueNode = pValueNode->NextSiblingElement( "value" ) )
			{
				PortParameterValue value;
				if( !loadValue( value, pValueNode->GetText() ) )
				{
					traceNodeError( pValueNode, "Error: value '"_s + pValueNode->GetText() + "' can't not be interpreted as '" + typeString + "'.\n" );
					return false;
				}

				m_values.pushBack( value );
			}

			if( m_defaultValue.type != PortParameterValueType::Invalid )
			{
				bool found = false;
				for( const PortParameterValue& value : m_values )
				{
					found |= isValueEquals( value, m_defaultValue );
				}

				if( !found )
				{
					traceNodeError( pParameterNode, "Error: default value is not in values set.\n" );
					return false;
				}
			}
		}

		return true;
	}

	bool PortParameter::loadValue( PortParameterValue& value, const char* pValueText ) const
	{
		if( m_type == PortParameterValueType::String )
		{
			value.type		= PortParameterValueType::String;
			value.string	= DynamicString( pValueText );

			return true;
		}
		else if( m_type == PortParameterValueType::Integer )
		{
			value.type		= PortParameterValueType::Integer;

			return string_tools::tryParseSInt32( value.integer, StringView( pValueText ) );
		}

		return false;
	}

	bool PortParameter::isValueEquals( const PortParameterValue& lhs, const PortParameterValue& rhs ) const
	{
		TIKI_ASSERT( lhs.type == m_type );
		TIKI_ASSERT( rhs.type == m_type );

		if( m_type == PortParameterValueType::String )
		{
			return lhs.string == rhs.string;
		}
		else if( m_type == PortParameterValueType::Integer )
		{
			return lhs.integer == rhs.integer;
		}

		return true;
	}

	bool PortParameter::matchFilters( const Device& device ) const
	{
		for( const PortParameterFilter& filter : m_filters )
		{
			switch( filter.type )
			{
			case PortParameterFilterType::HardwareSystem:
				if( device.getHardware().getSystem() != filter.system )
				{
					return false;
				}
				break;

			case PortParameterFilterType::HardwareEnvironment:
				if( device.getHardware().getEnvironment() != filter.environment )
				{
					return false;
				}
				break;

			default:
				break;
			}
		}

		return true;
	}
}
