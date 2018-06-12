#include "conct_dynamic_string.h"

#include "conct_ascii.h"
#include "conct_functions.h"
#include "conct_memory.h"
#include "conct_string.h"

namespace conct
{
	DynamicString::DynamicString()
		: m_pString( nullptr ), m_length( 0u ), m_capacity( 0u )
	{
	}

	DynamicString::DynamicString( const DynamicString& string )
		: m_pString( nullptr ), m_length( 0u ), m_capacity( 0u )
	{
		*this = string;
	}

	DynamicString::DynamicString( const char* pString )
		: m_pString( nullptr ), m_length( 0u ), m_capacity( 0u )
	{
		const uintreg stringLength = getStringLength( pString );
		checkCapacity( stringLength );
		memory::copy( m_pString, pString, stringLength );
		terminate( stringLength );
	}

	DynamicString::DynamicString( const char* pString, uintreg stringLength )
		: m_pString( nullptr ), m_length( 0u ), m_capacity( 0u )
	{
		CONCT_ASSERT( getStringLength( pString ) >= stringLength );

		checkCapacity( stringLength );
		memory::copy( m_pString, pString, stringLength );
		terminate( stringLength );
	}

	DynamicString::~DynamicString()
	{
		delete[] m_pString;
	}

	void DynamicString::clear()
	{
		if( m_capacity > 0u )
		{
			m_pString[ 0u ] = 0u;
		}

		m_length = 0u;
	}

	void DynamicString::reserve( uintreg size )
	{
		checkCapacity( size );
	}

	void DynamicString::terminate( uintreg newLength )
	{
		CONCT_ASSERT( newLength < m_capacity );
		m_length = newLength;
		m_pString[ m_length ] = '\0';
	}

	uintreg DynamicString::indexOf( char c, uintreg index /*= 0u*/ ) const
	{
		CONCT_ASSERT( index < m_length );

		uintreg i = index;
		while( i < m_length )
		{
			if( m_pString[ i ] == c )
			{
				return i;
			}
			i++;
		}

		return InvalidStringIndex;
	}

	uintreg DynamicString::indexOf( const DynamicString& str, uintreg index /*= 0u*/ ) const
	{
		if( str.m_length > m_length ) return InvalidStringIndex;

		uintreg i = index;
		uintreg c = m_length - str.m_length;

		do
		{
			uintreg b = 0;
			bool found = true;
			while( b < str.m_length )
			{
				if( m_pString[ i + b ] != str.m_pString[ b ] )
				{
					found = false;
					break;
				}
				b++;
			}

			if( found )
			{
				return i;
			}

			i++;
		} while( i <= c );

		return InvalidStringIndex;
	}

	uintreg DynamicString::lastIndexOf( char c, uintreg index /*= InvalidStringIndex*/ ) const
	{
		index = CONCT_MIN( index, m_length - 1u );
		while( index < m_length )
		{
			if( m_pString[ index ] == c )
			{
				return index;
			}

			index--;
		}

		return InvalidStringIndex;
	}

	uintreg DynamicString::lastIndexOf( const DynamicString& str, uintreg index /*= InvalidStringIndex*/ ) const
	{
		index = CONCT_MIN( index, m_length - 1u );
		while( index < m_length )
		{
			uintreg b = 0;
			bool found = true;
			while( b < str.m_length )
			{
				if( m_pString[ index + b ] != str.m_pString[ b ] )
				{
					found = false;
					break;
				}

				b++;
			}

			if( found )
			{
				return index;
			}

			index--;
		}

		return InvalidStringIndex;
	}

	bool DynamicString::contains( char c ) const
	{
		return indexOf( c ) != InvalidStringIndex;
	}

	bool DynamicString::contains( const DynamicString& str ) const
	{
		return indexOf( str ) != InvalidStringIndex;
	}

	bool DynamicString::startsWith( char c ) const
	{
		if( m_length < 1 ) return false;

		return m_pString[ 0 ] == c;
	}

	bool DynamicString::startsWith( const DynamicString& str ) const
	{
		if( m_length < str.m_length ) return false;

		uintreg i = 0;
		while( i < str.m_length )
		{
			if( m_pString[ i ] != str.m_pString[ i ] ) return false;
			i++;
		}

		return true;
	}

	bool DynamicString::endsWith( char c ) const
	{
		if( m_length < 1 ) return false;

		return m_pString[ m_length - 1 ] == c;
	}

	bool DynamicString::endsWith( const DynamicString& str ) const
	{
		if( m_length < str.m_length ) return false;

		uintreg b = 0;
		uintreg i = m_length - str.m_length;
		while( i < m_length )
		{
			if( m_pString[ i ] != str.m_pString[ b ] ) return false;
			i++;
			b++;
		}

		return true;
	}

	uintreg DynamicString::countSubstring( const DynamicString& substr ) const
	{
		if( substr.m_length > m_length )
			return 0u;

		uintreg index = 0;
		uintreg count = 0;
		while( index < m_length )
		{
			uintreg b = 0;
			bool found = true;
			while( b < substr.m_length )
			{
				if( m_pString[ index + b ] != substr.m_pString[ b ] )
				{
					found = false;
					break;
				}
				b++;
			}

			if( found )
			{
				count++;
				index += substr.m_length;
			}
			else
			{
				index++;
			}
		}

		return count;
	}

	DynamicString DynamicString::trim() const
	{
		uintreg startIndex = 0u;
		while( ascii::isWhitespace( m_pString[ startIndex ] ) && startIndex < getLength() )
		{
			startIndex++;
		}

		uintreg endIndex = getLength();
		while( ascii::isWhitespace( m_pString[ endIndex ] ) && endIndex > 0u )
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

	DynamicString DynamicString::subString( uintreg startIndex ) const
	{
		CONCT_ASSERT( startIndex <= m_length );
		return DynamicString( m_pString + startIndex, m_length - startIndex );
	}

	DynamicString DynamicString::subString( uintreg startIndex, uintreg length ) const
	{
		CONCT_ASSERT( startIndex + length <= m_length );
		return DynamicString( m_pString + startIndex, length );
	}

	DynamicString DynamicString::replace( char oldChar, char newChar ) const
	{
		DynamicString result = *this;
		for( uintreg i = 0u; i < m_length; ++i )
		{
			if( result[ i ] == oldChar )
			{
				result[ i ] = newChar;
			}
		}

		return result;
	}

	DynamicString DynamicString::replace( const DynamicString& oldString, const DynamicString& newString ) const
	{
		const uintreg count = countSubstring( oldString );
		const uintreg length = m_length - ( count * oldString.m_length ) + ( count * newString.m_length );

		if( count == 0 )
		{
			return *this;
		}

		DynamicString result;
		result.checkCapacity( length );

		uintreg i = 0;
		uintreg offsetOld = 0;
		uintreg offsetNew = 0;
		while( i < count )
		{
			const uintreg index			= indexOf( oldString, offsetOld );
			const uintreg oldDifferent	= index - offsetOld;

			memory::copy( result.m_pString + offsetNew, m_pString + offsetOld, index - offsetOld );
			offsetOld += oldDifferent;
			offsetNew += oldDifferent;

			memory::copy( result.m_pString + offsetNew, newString.m_pString, newString.m_length );
			offsetOld += oldString.m_length;
			offsetNew += newString.m_length;

			i++;
		}

		memory::copy( result.m_pString + offsetNew, m_pString + offsetOld, m_length - offsetOld );
		result.terminate( length );

		return result;
	}

	DynamicString DynamicString::insert( char c, uintreg index ) const
	{
		CONCT_ASSERT( index <= m_length );

		DynamicString result;
		result.checkCapacity( m_length + 1u );

		memory::copy( result.m_pString, m_pString, index );
		result.m_pString[ index ] = c;
		memory::copy( result.m_pString + index, m_pString, m_length - index );
		result.terminate( m_length + 1u );

		return result;
	}

	DynamicString DynamicString::erase( uintreg index ) const
	{
		CONCT_ASSERT( index <= m_length );
		CONCT_ASSERT( m_length > 0u );

		DynamicString result;
		result.checkCapacity( m_length - 1u );
		memory::copy( result.m_pString, m_pString, index );
		memory::copy( result.m_pString + index, m_pString, m_length - index );
		result.terminate( m_length - 1u );

		return result;
	}

	DynamicString DynamicString::popBack() const
	{
		DynamicString result;
		result.checkCapacity( m_length - 1u );
		memory::copy( result.m_pString, m_pString, m_length - 1u );
		result.terminate( m_length - 1u );

		return result;
	}

	DynamicString DynamicString::pushBack( char c ) const
	{
		DynamicString result;
		result.checkCapacity( m_length + 1u );
		memory::copy( result.m_pString, m_pString, m_length );
		result.m_pString[ m_length ] = c;
		result.terminate( m_length + 1u );

		return result;
	}

	char* DynamicString::toCharPointer()
	{
		return m_pString;
	}

	const char* DynamicString::toConstCharPointer() const
	{
		return m_pString;
	}

	char* DynamicString::getBegin()
	{
		return m_pString;
	}

	const char* DynamicString::getBegin() const
	{
		return m_pString;
	}

	char* DynamicString::getEnd()
	{
		return m_pString + m_length;
	}

	const char* DynamicString::getEnd() const
	{
		return m_pString + m_length;
	}

	char& DynamicString::operator[]( uintreg index )
	{
		CONCT_ASSERT( index < m_length );
		return m_pString[ index ];
	}

	const char& DynamicString::operator[]( uintreg index ) const
	{
		CONCT_ASSERT( index < m_length );
		return m_pString[ index ];
	}

	DynamicString& DynamicString::operator=( const DynamicString& rhs )
	{
		checkCapacity( rhs.m_length );
		memory::copy( m_pString, rhs.m_pString, rhs.m_length );
		terminate( rhs.m_length );

		return *this;
	}

	DynamicString& DynamicString::operator+=( const char* pString )
	{
		const uintreg stringLength = getStringLength( pString );

		checkCapacity( m_length + stringLength );
		memory::copy( m_pString + m_length, pString, stringLength );
		terminate( m_length + stringLength );

		return *this;
	}

	DynamicString& DynamicString::operator+=( const DynamicString& rhs )
	{
		checkCapacity( m_length + rhs.m_length );
		memory::copy( m_pString + m_length, rhs.m_pString, rhs.m_length );
		terminate( m_length + rhs.m_length );

		return *this;
	}

	DynamicString DynamicString::operator+( const char* pString ) const
	{
		const uintreg stringLength = getStringLength( pString );

		DynamicString result;
		result.checkCapacity( m_length + stringLength );
		memory::copy( result.m_pString, m_pString, m_length );
		memory::copy( result.m_pString + m_length, pString, stringLength );
		result.terminate( m_length + stringLength );

		return result;
	}

	DynamicString DynamicString::operator+( const DynamicString& rhs ) const
	{
		DynamicString result;
		result.checkCapacity( m_length + rhs.m_length );
		memory::copy( result.m_pString, m_pString, m_length );
		memory::copy( result.m_pString + m_length, rhs.m_pString, rhs.m_length );
		result.terminate( m_length + rhs.m_length );

		return result;
	}

	bool DynamicString::operator==( const DynamicString& rhs ) const
	{
		return isStringEquals( m_pString, rhs.m_pString );
	}

	bool DynamicString::operator==( const char* pString ) const
	{
		return isStringEquals( m_pString, pString );
	}

	bool DynamicString::operator!=( const DynamicString& rhs ) const
	{
		return !isStringEquals( m_pString, rhs.m_pString );
	}

	bool DynamicString::operator!=( const char* pString ) const
	{
		return !isStringEquals( m_pString, pString );
	}

	void DynamicString::checkCapacity( uintreg size )
	{
		const uintreg nextCapacity = getNextPowerOfTwo( size + 1u );
		if( nextCapacity < m_capacity )
		{
			return;
		}

		char* pNewString = new char[ nextCapacity ];
		CONCT_ASSERT( pNewString != nullptr );

		memory::copy( pNewString, m_pString, m_length );

		delete[] m_pString;
		m_pString = pNewString;
		m_capacity = nextCapacity;

		terminate( m_length );
	}

	DynamicString operator ""_s( const char* pString, size_t length )
	{
		return DynamicString( pString, length );
	}

	std::ostream& operator<<( std::ostream& o, const DynamicString& string )
	{
		return o << string.toConstCharPointer();
	}
}