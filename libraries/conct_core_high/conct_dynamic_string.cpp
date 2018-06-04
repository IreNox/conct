#include "conct_dynamic_string.h"

#include "conct_ascii.h"
#include "conct_string.h"

namespace conct
{
	DynamicString::DynamicString()
	{
	}

	DynamicString::DynamicString( const std::string& string )
		: m_string( string )
	{

	}

	DynamicString::DynamicString( const char* pString )
		: m_string( pString )
	{
	}

	DynamicString::DynamicString( const char* pString, uintreg stringLength )
	{
		m_string.reserve( stringLength );
		for( size_t i = 0u; i < stringLength; ++i )
		{
			m_string.push_back( pString[ i ] );
		}
	}

	DynamicString::~DynamicString()
	{
	}

	void DynamicString::clear()
	{
		m_string.clear();
	}

	bool DynamicString::isEmpty() const
	{
		return m_string.empty();
	}

	uintreg DynamicString::getLength() const
	{
		return m_string.length();
	}

	DynamicString DynamicString::trim() const
	{
		uintreg startIndex = 0u;
		while( ascii::isWhitespace( m_string[ startIndex ] ) && startIndex < getLength() )
		{
			startIndex++;
		}

		uintreg endIndex = getLength();
		while( ascii::isWhitespace( m_string[ endIndex ] ) && endIndex > 0u )
		{
			endIndex--;
		}

		return subString( startIndex, endIndex - startIndex );
	}

	DynamicString DynamicString::toLower() const
	{
		DynamicString result = *this;
		for( size_t i = 0u; i < result.getLength(); ++i )
		{
			result[ i ] = ascii::toLower( result[ i ] );
		}

		return result;
	}

	DynamicString DynamicString::toUpper() const
	{
		DynamicString result = *this;
		for( size_t i = 0u; i < result.getLength(); ++i )
		{
			result[ i ] = ascii::toUpper( result[ i ] );
		}

		return result;
	}

	DynamicString DynamicString::subString( uintreg startIndex, uintreg length ) const
	{
		return DynamicString( m_string.c_str() + startIndex, length );
	}

	DynamicString DynamicString::insert( char c, uintreg index ) const
	{
		DynamicString result = *this;
		result.m_string.insert( result.m_string.begin() + index, c );
		return result;
	}

	DynamicString DynamicString::erase( uintreg index ) const
	{
		DynamicString result = *this;
		result.m_string.erase( result.m_string.begin() + index );
		return result;
	}

	DynamicString DynamicString::popBack() const
	{
		DynamicString result = *this;
		result.m_string.pop_back();
		return result;
	}

	DynamicString DynamicString::pushBack( char c ) const
	{
		DynamicString result = *this;
		result.m_string.push_back( c );
		return result;
	}

	char* DynamicString::toCharPointer()
	{
		return &*m_string.begin();
	}

	const char* DynamicString::toConstCharPointer() const
	{
		return m_string.c_str();
	}

	char& DynamicString::operator[]( uintreg index )
	{
		return m_string[ index ];
	}

	const char& DynamicString::operator[]( uintreg index ) const
	{
		return m_string[ index ];
	}

	DynamicString& DynamicString::operator+=( const DynamicString& rhs )
	{
		m_string += rhs.m_string;
		return *this;
	}

	DynamicString& DynamicString::operator+=( const char* pString )
	{
		m_string += pString;
		return *this;
	}

	DynamicString DynamicString::operator+( const char* pString ) const
	{
		DynamicString result = *this;
		result.m_string += pString;
		return result;
	}

	DynamicString DynamicString::operator+( const DynamicString& rhs ) const
	{
		DynamicString result = *this;
		result.m_string += rhs.m_string;
		return result;
	}

	bool DynamicString::operator!=( const DynamicString& rhs ) const
	{
		return m_string != rhs.m_string;
	}

	bool DynamicString::operator==( const DynamicString& rhs ) const
	{
		return m_string == rhs.m_string;
	}

	bool DynamicString::operator==( const char* pString ) const
	{
		return isStringEquals( m_string.c_str(), pString );
	}

	DynamicString operator ""_s( const char* pString, size_t length )
	{
		return DynamicString( pString, length );
	}
}