#include "console_input.h"

#include "conct_core.h"

#include <windows.h>

namespace conct
{
	static const int s_aKeyMapping[] =
	{
		VK_ESCAPE,	// ConsoleKey_Escape
		VK_RETURN,	// ConsoleKey_Return
		'\b',		// ConsoleKey_Backspace
		'\t',		// ConsoleKey_Tab
		VK_F1,		// ConsoleKey_F1
		VK_F2,		// ConsoleKey_F2
		VK_F3,		// ConsoleKey_F3
		VK_F4,		// ConsoleKey_F4
		VK_F5,		// ConsoleKey_F5
		VK_F6,		// ConsoleKey_F6
		VK_F7,		// ConsoleKey_F7
		VK_F8,		// ConsoleKey_F8
		VK_F9,		// ConsoleKey_F9
		VK_F10,		// ConsoleKey_F10
		VK_F11,		// ConsoleKey_F11
		VK_F12,		// ConsoleKey_F12
		'0',		// ConsoleKey_0
		'1',		// ConsoleKey_1
		'2',		// ConsoleKey_2
		'3',		// ConsoleKey_3
		'4',		// ConsoleKey_4
		'5',		// ConsoleKey_5
		'6',		// ConsoleKey_6
		'7',		// ConsoleKey_7
		'8',		// ConsoleKey_8
		'9',		// ConsoleKey_9
		VK_UP,		// ConsoleKey_Up
		VK_DOWN,	// ConsoleKey_Down
		VK_LEFT,	// ConsoleKey_Left
		VK_RIGHT,	// ConsoleKey_Right
	};
	static_assert( CONCT_COUNT( s_aKeyMapping ) == ConsoleKey_Count, "" );

	void ConsoleInput::setup()
	{
	}

	bool ConsoleInput::getKeyState( ConsoleKey key )
	{
		return GetAsyncKeyState( s_aKeyMapping[ key ] );
	}

	bool ConsoleInput::readKey( ConsoleKey& key )
	{
		const HANDLE consoleHandle = GetStdHandle( STD_INPUT_HANDLE );
		INPUT_RECORD input;
		DWORD read;
		PeekConsoleInputA( consoleHandle, &input, 1u, &read );
		if( read > 0u )
		{
			if( input.EventType == KEY_EVENT && input.Event.KeyEvent.bKeyDown )
			{
				const char c = input.Event.KeyEvent.uChar.AsciiChar;
				if( c < ' ' )
				{
					ReadConsoleInputA( consoleHandle, &input, 1u, &read );

					for( size_t i = 0u; i < CONCT_COUNT( s_aKeyMapping ); ++i )
					{
						if( s_aKeyMapping[ i ] == input.Event.KeyEvent.wVirtualKeyCode )
						{
							key = ( ConsoleKey )i;
							return true;
						}
					}
				}

				return false;
			}

			ReadConsoleInputA( consoleHandle, &input, 1u, &read );
		}

		return false;
	}

	bool ConsoleInput::readChar( char& c )
	{
		const HANDLE consoleHandle = GetStdHandle( STD_INPUT_HANDLE );
		INPUT_RECORD input;
		DWORD read;
		PeekConsoleInputA( consoleHandle, &input, 1u, &read );
		if( read > 0u )
		{
			if( input.EventType == KEY_EVENT && input.Event.KeyEvent.bKeyDown )
			{
				c = input.Event.KeyEvent.uChar.AsciiChar;
				if( c >= ' ' && c <= '~' )
				{
					ReadConsoleInputA( consoleHandle, &input, 1u, &read );
					return true;
				}

				return false;
			}

			ReadConsoleInputA( consoleHandle, &input, 1u, &read );
		}

		return false;
	}
}
