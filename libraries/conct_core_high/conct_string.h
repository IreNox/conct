#pragma once

#include "conct_core.h"

namespace conct
{
	class String
	{
	public:

					String();
		explicit	String( const char* pString );
		explicit	String( const char* pString, uintreg stringLength );
					~String();

		char&		operator[]( uintreg index );
		const char&	operator[]( uintreg index ) const;

		bool		operator==( const String& rhs ) const;
		bool		operator!=( const String& rhs ) const;

		String&		operator+=( const char* pString );
		String&		operator+=( const String& rhs );

	private:

		std::string		m_string;
	};
}
