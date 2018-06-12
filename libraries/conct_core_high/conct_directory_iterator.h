#pragma once

#include "conct_path.h"

namespace conct
{
	class DirectoryIterator
	{
	public:

						DirectoryIterator( const Path& path );
						DirectoryIterator( const DynamicString& path );
						~DirectoryIterator();

		bool			isEnd() const { return m_currentPath.isEmpty(); }

		bool			next();

		inline Path		getCurrent() const { return m_currentPath; }

	private:

		Path			m_iteratorPath;
		Path			m_currentPath;

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		void*			m_pSearchHandle;
#endif

		void			initialize();
		void			shutdown();
	};
}