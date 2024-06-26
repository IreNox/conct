#pragma once

#include "conct_result.h"

#include <tiki/tiki_dynamic_string.h>

namespace conct
{
	namespace filesystem
	{
		bool						exists( const Path& path );

		bool						isFile( const Path& path );
		bool						isDirectory( const Path& path );

		Path						getExecutablePath();

		Result< void >				createDirectory( const Path& path );

		Result< DynamicString >		readTextFile( const Path& path );
		Result< DynamicArray< byte > >	readBinaryFile( const Path& path );

		Result< void >				writeTextFile( const Path& path, const DynamicString& text );
		Result< void >				writeBinaryFile( const Path& path, const DynamicArray< byte >& data );
	}
}