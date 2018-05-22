#pragma once

#include "conct_core.h"

#include <string>

namespace conct
{
	class String
	{
	public:

						String();
						String( const std::string& string ); // please delete this later
		explicit		String( const char* pString );
		explicit		String( const char* pString, uintreg stringLength );
						~String();

		void			clear();

		bool			isEmpty() const;

		uintreg			getLength() const;

		String			trim() const;
		String			toLower() const;
		String			toUpper() const;
		String			subString( uintreg startIndex, uintreg length ) const;

		String			insert( char c, uintreg index ) const;
		String			erase( uintreg index ) const;

		String			popBack() const;
		String			pushBack( char c ) const;

		char*			toCharPointer();
		const char*		toConstCharPointer() const;

		char&			operator[]( uintreg index );
		const char&		operator[]( uintreg index ) const;

		bool			operator==( const String& rhs ) const;
		bool			operator!=( const String& rhs ) const;

		String&			operator+=( const char* pString );
		String&			operator+=( const String& rhs );

	private:

		std::string		m_string;
	};

	String operator ""_s( const char* pString, size_t length );
}
