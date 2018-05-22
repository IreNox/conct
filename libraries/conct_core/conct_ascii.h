#pragma once

namespace conct
{
	namespace ascii
	{
		bool	isAlphaNumeric( char c );
		bool	isLetter( char c );
		bool	isCapital( char c );
		bool	isMinuscule( char c );
		bool	isNumber( char c );
		bool	isSymbol( char c );

		bool	isWhitespace( char c );
		bool	isSpace( char c );
		bool	isTab( char c );
		bool	isNewLine( char c );

		char	toLower( char c );
		char	toUpper( char c );
	}
}