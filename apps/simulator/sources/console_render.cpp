#include "console_render.h"

#include <windows.h>

namespace conct
{
	void ConsoleRenderer::setup()
	{
		const HANDLE consoleHandle = GetStdHandle( STD_INPUT_HANDLE );
		DWORD modes;
		GetConsoleMode( consoleHandle, &modes );
		SetConsoleMode( consoleHandle, modes | ENABLE_WINDOW_INPUT );
	}

	uint16x2 ConsoleRenderer::measureTextSize( const char* pString )
	{
		const uint16 length = uint16( strlen( pString ) );

		uint16 x = 0u;
		uint16x2 size = { 0u, 0u };
		for( uint16 i = 0u; i < length; ++i )
		{
			const char c = pString[ i ];
			if( c == '\t' )
			{
				x += 4u;
				continue;
			}
			else if( c == '\n' )
			{
				size.x = CONCT_MAX( size.x, x );
				size.y++;

				x = 0u;

				continue;
			}

			x++;
		}

		size.x = CONCT_MAX( size.x, x );
		return size;
	}

	uint16 ConsoleRenderer::drawButton( uint16 x, uint16 y, const char* pText, ConsoleLineType type )
	{
		const uint16 width = uint16( strlen( pText ) + 4u );

		drawRectangle( x, y, width, 3u, type );
		drawText( x + 2u, y + 1u, pText );

		return width;
	}

	void ConsoleRenderer::drawRectangle( uint16 x, uint16 y, uint16 width, uint16 height, ConsoleLineType type )
	{
		static const wchar_t* s_apChars[] =
		{
			L"┌┐└┘─│",
			L"╔╗╚╝═║"
		};
		const wchar_t* pType = s_apChars[ type ];

		drawCharacter( x, y, pType[ 0u ] );
		drawCharacter( x + width - 1u, y, pType[ 1u ] );
		drawCharacter( x, y + height - 1u, pType[ 2u ] );
		drawCharacter( x + width - 1u, y + height - 1u, pType[ 3u ] );

		if( width > 2u )
		{
			drawCharacterRepeated( x + 1u, y, pType[ 4u ], width - 2u );
			drawCharacterRepeated( x + 1u, y + height - 1u, pType[ 4u ], width - 2u );
		}

		for( uint i = 0u; i < height - 2u; ++i )
		{
			drawCharacter( x, y + i + 1u, pType[ 5u ] );
			drawCharacter( x + width - 1u, y + i + 1u, pType[ 5u ] );
		}
	}

	void ConsoleRenderer::drawFillRectangle( uint16 x, uint16 y, uint16 width, uint16 height, wchar_t c )
	{
		for( uint i = 0u; i < height; ++i )
		{
			drawCharacterRepeated( x, y + i, c, width );
		}
	}

	void ConsoleRenderer::drawText( uint16 x, uint16 y, const char* pString )
	{
		if( pString == nullptr )
		{
			return;
		}

		const HANDLE consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

		COORD coord;
		coord.X = x;
		coord.Y = y;

		DWORD written = 0u;
		WriteConsoleOutputCharacterA( consoleHandle, pString, ( DWORD )strlen( pString ), coord, &written );
	}

	uint16x2 ConsoleRenderer::drawTextMultiline( uint16 x, uint16 y, const char* pString )
	{
		const uint16 baseX = x;
		const uint16 length = uint16( strlen( pString ) );

		uint16x2 size = { 0u, 1u };
		for( uint16 i = 0u; i < length; ++i )
		{
			const char c = pString[ i ];
			if( c == '\t' )
			{
				const uint16 xm4 = 4u - (x - baseX) % 4u;
				x += xm4;
				continue;
			}
			else if( c == '\n' )
			{
				size.x = CONCT_MAX( size.x, x - baseX );
				size.y++;

				x = baseX;
				continue;
			}

			drawCharacter( x, y + size.y, c );
			x++;
		}

		size.x = CONCT_MAX( size.x, x - baseX );
		return size;
	}

	void ConsoleRenderer::drawCharacter( uint16 x, uint16 y, wchar_t c )
	{
		drawCharacterRepeated( x, y, c, 1u );
	}

	void ConsoleRenderer::drawCharacterRepeated( uint16 x, uint16 y, wchar_t c, uint16 count )
	{
		const HANDLE consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

		COORD coord;
		coord.X = x;
		coord.Y = y;

		DWORD written = 0u;
		FillConsoleOutputCharacterW( consoleHandle, c, count, coord, &written );
	}

	uint16x2 ConsoleRenderer::getSize()
	{
		const HANDLE consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

		CONSOLE_SCREEN_BUFFER_INFO info;
		GetConsoleScreenBufferInfo( consoleHandle, &info );

		const uint16x2 size = {
			uint16( info.srWindow.Right - info.srWindow.Left ),
			uint16( info.srWindow.Bottom - info.srWindow.Top )
		};
		return size;
	}

	void ConsoleRenderer::setCursorPosition( uint16 x, uint16 y )
	{
		const HANDLE consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

		COORD coord;
		coord.X = x;
		coord.Y = y;

		SetConsoleCursorPosition( consoleHandle, coord );
	}
}
