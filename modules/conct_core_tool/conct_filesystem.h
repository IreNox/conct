#pragma once

#include "conct_result.h"

#include <tiki/tiki_dynamic_string.h>

namespace conct
{
	class Path;

	namespace filesystem
	{
		bool						exists( const Path& path );

		bool						isFile( const Path& path );
		bool						isDirectory( const Path& path );

		Result< void >				createDirectory( const Path& path );

		Result< DynamicString >		readTextFile( const Path& path );
		Result< DynamicArray< uint8 > >	readBinaryFile( const Path& path );

		Result< void >				writeTextFile( const Path& path, const DynamicString& text );
		Result< void >				writeBinaryFile( const Path& path, const DynamicArray< uint8 >& data );
	}
}