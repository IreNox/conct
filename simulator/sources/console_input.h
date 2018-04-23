#pragma once

namespace conct
{
	enum ConsoleKey
	{
		ConsoleKey_Escape,
		ConsoleKey_Return,
		ConsoleKey_Backspace,
		ConsoleKey_Tab,
		ConsoleKey_F1,
		ConsoleKey_F2,
		ConsoleKey_F3,
		ConsoleKey_F4,
		ConsoleKey_F5,
		ConsoleKey_F6,
		ConsoleKey_F7,
		ConsoleKey_F8,
		ConsoleKey_F9,
		ConsoleKey_F10,
		ConsoleKey_F11,
		ConsoleKey_F12,
		ConsoleKey_0,
		ConsoleKey_1,
		ConsoleKey_2,
		ConsoleKey_3,
		ConsoleKey_4,
		ConsoleKey_5,
		ConsoleKey_6,
		ConsoleKey_7,
		ConsoleKey_8,
		ConsoleKey_9,
		ConsoleKey_Up,
		ConsoleKey_Down,
		ConsoleKey_Left,
		ConsoleKey_Right,

		ConsoleKey_Count
	};

	class ConsoleInput
	{
	public:

		static void		setup();

		static bool		getKeyState( ConsoleKey key );

		static bool		readKey( ConsoleKey& key );
		static bool		readChar( char& c );
	};
}