#pragma once

#include "tiki/tiki_path.h"

#if TIKI_ENABLED( TIKI_PLATFORM_LINUX )
struct __dirstream;
typedef __dirstream DIR;
#elif TIKI_ENABLED( TIKI_PLATFORM_ANDROID )
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

		bool				isEnd() const { return m_currentPath.isEmpty(); }

		bool				next();

		inline const Path&	getCurrent() const { return m_currentPath; }

	private:

		Path				m_iteratorPath;
		Path				m_currentPath;

#if TIKI_ENABLED( TIKI_PLATFORM_WINDOWS )
		void*				m_pSearchHandle;
#elif TIKI_ENABLED( TIKI_PLATFORM_LINUX ) || TIKI_ENABLED( TIKI_PLATFORM_ANDROID )
		DIR*				m_pDir;
#endif

		void				initialize();
		void				shutdown();
	};
}