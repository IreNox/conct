#include "conct_string.h"

#include "conct_ascii.h"

namespace conct
{

	String::String()
	{
	}

	String::String( const std::string& string )
		: m_string( string )
	{

	}

	String::String( const char* pString )
		: m_string( pString )
	{
	}

	String::String( const char* pString, uintreg stringLength )
	{
		m_string.reserve( stringLength );
		for( size_t i = 0u; i < stringLength; ++i )
		{
			m_string.push_back( pString[ i ] );
		}
	}

	String::~String()
	{
	}

	void String::clear()
	{
		m_string.clear();
	}

	bool String::isEmpty() const
	{
		return m_string.empty();
	}

	uintreg String::getLength() const
	{
		return m_string.length();
	}

	String String::trim() const
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

	String String::toLower() const
	{
		String result = *this;
		for( size_t i = 0u; i < result.getLength(); ++i )
		{
			result[ i ] = ascii::toLower( result[ i ] );
		}

		return result;
	}

	String String::toUpper() const
	{
		String result = *this;
		for( size_t i = 0u; i < result.getLength(); ++i )
		{
			result[ i ] = ascii::toUpper( result[ i ] );
		}

		return result;
	}

	String String::subString( uintreg startIndex, uintreg length ) const
	{
		return String( m_string.c_str() + startIndex, length );
	}

	String String::insert( char c, uintreg index ) const
	{
		String result = *this;
		result.m_string.insert( result.m_string.begin() + index, c );
		return result;
	}

	String String::erase( uintreg index ) const
	{
		String result = *this;
		result.m_string.erase( result.m_string.begin() + index );
		return result;
	}

	String String::popBack() const
	{
		String result = *this;
		result.m_string.pop_back();
		return result;
	}

	String String::pushBack( char c ) const
	{
		String result = *this;
		result.m_string.push_back( c );
		return result;
	}

	char* String::toCharPointer()
	{
		return &*m_string.begin();
	}

	const char* String::toConstCharPointer() const
	{
		return m_string.c_str();
	}

	char& String::operator[]( uintreg index )
	{
		return m_string[ index ];
	}

	const char& String::operator[]( uintreg index ) const
	{
		return m_string[ index ];
	}

	String& String::operator+=( const String& rhs )
	{
		m_string += rhs.m_string;
		return *this;
	}

	String& String::operator+=( const char* pString )
	{
		m_string += pString;
		return *this;
	}

	bool String::operator!=( const String& rhs ) const
	{
		return m_string != rhs.m_string;
	}

	bool String::operator==( const String& rhs ) const
	{
		return m_string == rhs.m_string;
	}

	String operator ""_s( const char* pString, size_t length )
	{
		return String( pString, length );
	}
}