#pragma once

#include "conct_core.h"
#include "conct_structs.h"

namespace conct
{
	enum ConsoleLineType
	{
		LineType_Single,
		LineType_Double,

		LineType_Count
	};

	class ConsoleRenderer
	{
	public:

		static void			setup();

		static uint16x2		measureTextSize( const char* pString );

		static uint16		drawButton( uint16 x, uint16 y, const char* pText, ConsoleLineType type );
		static void			drawRectangle( uint16 x, uint16 y, uint16 width, uint16 height, ConsoleLineType type );
		static void			drawFillRectangle( uint16 x, uint16 y, uint16 width, uint16 height, wchar_t c );

		static void			drawText( uint16 x, uint16 y, const char* pString );
		static uint16x2		drawTextMultiline( uint16 x, uint16 y, const char* pString );

		static void			drawCharacter( uint16 x, uint16 y, wchar_t c );
		static void			drawCharacterRepeated( uint16 x, uint16 y, wchar_t c, uint16 count );

		static uint16x2		getSize();

		static void			setCursorPosition( uint16 x, uint16 y );
	};
}