#pragma once

#include "conct_core.h"

#include <string>

namespace conct
{
	class DynamicString
	{
	public:

							DynamicString();
							DynamicString( const std::string& string ); // please delete this later
		explicit			DynamicString( const char* pString );
		explicit			DynamicString( const char* pString, uintreg stringLength );
							~DynamicString();

		void				clear();

		bool				isEmpty() const;

		uintreg				getLength() const;

		DynamicString		trim() const;
		DynamicString		toLower() const;
		DynamicString		toUpper() const;
		DynamicString		subString( uintreg startIndex, uintreg length ) const;

		DynamicString		insert( char c, uintreg index ) const;
		DynamicString		erase( uintreg index ) const;

		DynamicString		popBack() const;
		DynamicString		pushBack( char c ) const;

		char*				toCharPointer();
		const char*			toConstCharPointer() const;

		char&				operator[]( uintreg index );
		const char&			operator[]( uintreg index ) const;

		bool				operator==( const DynamicString& rhs ) const;
		bool				operator==( const char* pString ) const;
		bool				operator!=( const DynamicString& rhs ) const;

		DynamicString&		operator+=( const char* pString );
		DynamicString&		operator+=( const DynamicString& rhs );

		DynamicString		operator+( const char* pString ) const;
		DynamicString		operator+( const DynamicString& rhs ) const;

	private:

		std::string			m_string;
	};

	DynamicString operator ""_s( const char* pDynamicString, size_t length );
}
