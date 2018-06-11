#pragma once

#include "conct_string.h"

#include <ostream>

namespace conct
{
	class DynamicString
	{
	public:

							DynamicString();
		explicit			DynamicString( const char* pString );
		explicit			DynamicString( const char* pString, uintreg stringLength );
							~DynamicString();

		void				clear();

		bool				isEmpty() const;

		uintreg				getLength() const;

		uintreg				indexOf( char c, uintreg index = 0u ) const;
		uintreg				indexOf( const DynamicString& str, uintreg index = 0u ) const;

		uintreg				lastIndexOf( char c, uintreg index = InvalidStringIndex ) const;
		uintreg				lastIndexOf( const DynamicString& str, uintreg index = InvalidStringIndex ) const;

		bool				contains( char c ) const;
		bool				contains( const DynamicString& str ) const;

		bool				startsWith( char c ) const;
		bool				startsWith( const DynamicString& str ) const;

		bool				endsWith( char c ) const;
		bool				endsWith( const DynamicString& str ) const;

		uintreg				countSubstring( const DynamicString& substr ) const;

		DynamicString		trim() const;
		DynamicString		toLower() const;
		DynamicString		toUpper() const;

		DynamicString		subString( uintreg startIndex ) const;
		DynamicString		subString( uintreg startIndex, uintreg length ) const;

		DynamicString		replace( char oldChar, char newChar ) const;
		DynamicString		replace( const DynamicString& oldString, const DynamicString& newString ) const;

		DynamicString		insert( char c, uintreg index ) const;
		DynamicString		erase( uintreg index ) const;

		DynamicString		popBack() const;
		DynamicString		pushBack( char c ) const;

		char*				toCharPointer();
		const char*			toConstCharPointer() const;

		char&				operator[]( uintreg index );
		const char&			operator[]( uintreg index ) const;

		DynamicString&		operator=( const DynamicString& rhs );

		bool				operator==( const DynamicString& rhs ) const;
		bool				operator==( const char* pString ) const;
		bool				operator!=( const DynamicString& rhs ) const;

		DynamicString&		operator+=( const char* pString );
		DynamicString&		operator+=( const DynamicString& rhs );

		DynamicString		operator+( const char* pString ) const;
		DynamicString		operator+( const DynamicString& rhs ) const;

	private:

		char*				m_pString;
		uintreg				m_capacity;
		uintreg				m_length;

		void				checkCapacity( uintreg size );
		void				terminate();
		void				terminate( uintreg newLength );
	};

	DynamicString operator ""_s( const char* pString, size_t length );

	std::ostream& operator<<( std::ostream& o, const DynamicString& string );
}
