#pragma once

#include "conct_result.h"
#include "conct_dynamic_string.h"
#include "conct_vector.h"

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
		Result< Vector< uint8 > >	readBinaryFile( const Path& path );

		Result< void >				writeTextFile( const Path& path, const DynamicString& text );
		Result< void >				writeBinaryFile( const Path& path, const Vector< uint8 >& data );
	}
}