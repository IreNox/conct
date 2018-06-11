#pragma once

namespace conct
{
	class Path;

	namespace filesystem
	{
		bool	exists( const Path& path );

		bool	isFile( const Path& path );
		bool	isDirectory( const Path& path );

		bool	createDirectory( const Path& path );
	}
}