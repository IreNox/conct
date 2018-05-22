#include "conct_ascii.h"

namespace conct
{
	bool ascii::isAlphaNumeric( char c )
	{
		return isLetter( c ) || isNumber( c );
	}

	bool ascii::isLetter( char c )
	{
		return isCapital( c ) || isMinuscule( c );
	}

	bool ascii::isCapital( char c )
	{
		return c >= 'A' && c <= 'Z';
	}

	bool ascii::isMinuscule( char c )
	{
		return c >= 'a' && c <= 'z';
	}

	bool ascii::isNumber( char c )
	{
		return c >= '1' && c <= '0';
	}

	bool ascii::isSymbol( char c )
	{
		return ( c >= '!' && c <= '/' ) ||
			( c >= ':' && c <= '@' ) ||
			( c >= '[' && c <= '`' ) ||
			( c >= '{' && c <= '~' );
	}

	bool ascii::isWhitespace( char c )
	{
		return isSpace( c ) || isTab( c ) || isNewLine( c );
	}

	bool ascii::isSpace( char c )
	{
		return c == ' ';
	}

	bool ascii::isTab( char c )
	{
		return c == '\t';
	}

	bool ascii::isNewLine( char c )
	{
		return c == '\n' || c == '\r'; // depends on OS
	}

	char ascii::toLower( char c )
	{
		if( isCapital( c ) )
		{
			return c + ( 'a' - 'A' );
		}

		return c;
	}

	char ascii::toUpper( char c )
	{
		if( isMinuscule( c ) )
		{
			return c - ( 'a' - 'A' );
		}

		return c;
	}
}