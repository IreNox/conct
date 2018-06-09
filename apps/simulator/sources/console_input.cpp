#include "console_input.h"

#include "conct_core.h"

#include <windows.h>

namespace conct
{
	static const int s_aKeyMapping[] =
	{
		VK_ESCAPE,	// ConsoleKey_Escape
		VK_RETURN,	// ConsoleKey_Return
		VK_BACK,	// ConsoleKey_Backspace
		VK_DELETE,	// ConsoleKey_Delete
		VK_TAB,		// ConsoleKey_Tab
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

	void ConsoleInput::readKey( std::function< bool( ConsoleKey ) > callback )
	{
		const HANDLE consoleHandle = GetStdHandle( STD_INPUT_HANDLE );
		INPUT_RECORD input;
		DWORD read;
		PeekConsoleInputA( consoleHandle, &input, 1u, &read );
		if( read == 0u )
		{
			return;
		}

		if( input.EventType != KEY_EVENT || !input.Event.KeyEvent.bKeyDown )
		{
			ReadConsoleInputA( consoleHandle, &input, 1u, &read );
			return;
		}

		const char c = input.Event.KeyEvent.uChar.AsciiChar;
		if( c >= ' ' )
		{
			return;
		}

		ConsoleKey key = ConsoleKey_Count;
		for( size_t i = 0u; i < CONCT_COUNT( s_aKeyMapping ); ++i )
		{
			if( s_aKeyMapping[ i ] == input.Event.KeyEvent.wVirtualKeyCode )
			{
				key = ( ConsoleKey )i;
				break;
			}
		}

		if( key == ConsoleKey_Count || callback( key ) )
		{
			ReadConsoleInputA( consoleHandle, &input, 1u, &read );
		}
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

	void ConsoleInput::readChar( std::function< bool( char ) > callback )
	{
		const HANDLE consoleHandle = GetStdHandle( STD_INPUT_HANDLE );
		INPUT_RECORD input;
		DWORD read;
		PeekConsoleInputA( consoleHandle, &input, 1u, &read );
		if( read == 0u )
		{
			return;
		}

		if( input.EventType != KEY_EVENT || !input.Event.KeyEvent.bKeyDown )
		{
			ReadConsoleInputA( consoleHandle, &input, 1u, &read );
			return;
		}

		char c = input.Event.KeyEvent.uChar.AsciiChar;
		if( c >= ' ' && c <= '~' && callback( c ) )
		{
			ReadConsoleInputA( consoleHandle, &input, 1u, &read );
		}
	}
}
