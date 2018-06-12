#pragma once

#include "conct_path.h"

#if CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
struct DIR;
#endif

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
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
		DIR*			m_pDir;
#endif

		void			initialize();
		void			shutdown();
	};
}