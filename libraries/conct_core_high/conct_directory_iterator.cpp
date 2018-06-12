#include "conct_directory_iterator.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	include <windows.h>
#endif

namespace conct
{
	DirectoryIterator::DirectoryIterator( const Path& path )
	{
		m_iteratorPath = path;
		initialize();
	}

	DirectoryIterator::DirectoryIterator( const DynamicString& path )
	{
		m_iteratorPath = Path( path );
		initialize();
	}

	DirectoryIterator::~DirectoryIterator()
	{
		shutdown();
	}

	bool DirectoryIterator::next()
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		WIN32_FIND_DATAA findData;
		if( m_pSearchHandle == INVALID_HANDLE_VALUE )
		{
			DynamicString searchPath = m_iteratorPath.getNativePath() + "\\*.*";
			m_pSearchHandle = FindFirstFileA( searchPath.toConstCharPointer(), &findData );

			if( m_pSearchHandle == INVALID_HANDLE_VALUE )
			{
				m_currentPath.clear();
				return false;
			}
		}
		else
		{
			if( !FindNextFileA( m_pSearchHandle, &findData ) )
			{
				m_currentPath.clear();
				shutdown();
				return false;
			}
		}

		if( (findData.cFileName[ 0u ] == '.' && findData.cFileName[ 1u ] == '\0') ||
			(findData.cFileName[ 0u ] == '.' && findData.cFileName[ 1u ] == '.' && findData.cFileName[ 2u ] == '\0' ) )
		{
			return next();
		}

		m_currentPath = m_iteratorPath.push( DynamicString( findData.cFileName ) );
		return true;
#endif
	}

	void DirectoryIterator::initialize()
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		m_pSearchHandle = INVALID_HANDLE_VALUE;
#endif
	}

	void DirectoryIterator::shutdown()
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		if( m_pSearchHandle != INVALID_HANDLE_VALUE )
		{
			FindClose( m_pSearchHandle );
			m_pSearchHandle = INVALID_HANDLE_VALUE;
		}
#endif
	}
}