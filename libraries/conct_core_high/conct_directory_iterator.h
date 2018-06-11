#pragma once

#include "conct_dynamic_string.h"

namespace conct
{
	class Path;

	class DirectoryIterator
	{
	public:

						DirectoryIterator( const Path& path );
						DirectoryIterator( const DynamicString& path );

		bool			isEnd() const;

		bool			next();

		Path			getCurrent() const;

	private:

		DynamicString	m_path;
	};
}