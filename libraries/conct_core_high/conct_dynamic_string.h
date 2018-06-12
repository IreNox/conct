#pragma once

#include "conct_string.h"

#include <ostream>

namespace conct
{
	class DynamicString
	{
	public:

							DynamicString();
							DynamicString( const DynamicString& string );
		explicit			DynamicString( const char* pString );
		explicit			DynamicString( const char* pString, uintreg stringLength );
							~DynamicString();

		bool				isEmpty() const { return m_length == 0u;}

		uintreg				getLength() const { return m_length; }
		uintreg				getCapacity() const { return m_capacity; }

		void				clear();
		void				reserve( uintreg size );
		void				terminate( uintreg newLength );

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

		char*				getBegin();
		const char*			getBegin() const;
		char*				getEnd();
		const char*			getEnd() const;

		char&				operator[]( uintreg index );
		const char&			operator[]( uintreg index ) const;

		DynamicString&		operator=( const DynamicString& rhs );

		bool				operator==( const DynamicString& rhs ) const;
		bool				operator==( const char* pString ) const;
		bool				operator!=( const DynamicString& rhs ) const;
		bool				operator!=( const char* pString ) const;

		DynamicString&		operator+=( const char* pString );
		DynamicString&		operator+=( const DynamicString& rhs );

		DynamicString		operator+( const char* pString ) const;
		DynamicString		operator+( const DynamicString& rhs ) const;

		char*				begin() { return getBegin(); }
		const char*			begin() const { return getBegin(); }
		char*				end() { return getEnd(); }
		const char*			end() const { return getEnd(); }

	private:

		char*				m_pString;
		uintreg				m_capacity;
		uintreg				m_length;

		void				checkCapacity( uintreg size );
	};

	DynamicString operator ""_s( const char* pString, size_t length );

	std::ostream& operator<<( std::ostream& o, const DynamicString& string );
}
