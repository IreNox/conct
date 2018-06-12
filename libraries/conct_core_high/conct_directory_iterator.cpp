#include "conct_directory_iterator.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	include <windows.h>
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
#	include <dirent.h>
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
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
		if( m_pDir == nullptr )
		{
			m_pDir = opendir( m_iteratorPath.getNativePath().toConstCharPointer() );
			if( m_pDir == nullptr )
			{
				m_currentPath.clear();
				return false;
			}
		}

		dirent* pDirEntry = readdir( m_pDir );
		if( pDirEntry == nullptr )
		{
			shutdown();
			return false;
		}

		if( ( pDirEntry->d_name[ 0u ] == '.' && pDirEntry->d_name[ 1u ] == '\0' ) ||
			( pDirEntry->d_name[ 0u ] == '.' && pDirEntry->d_name[ 1u ] == '.' && pDirEntry->d_name[ 2u ] == '\0' ) )
		{
			return next();
		}

		m_currentPath = m_iteratorPath.push( DynamicString( pDirEntry->d_name ) );
		return true;
#endif
	}

	void DirectoryIterator::initialize()
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		m_pSearchHandle = INVALID_HANDLE_VALUE;
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
		m_pDir = nullptr;
#endif
	}

	void DirectoryIterator::shutdown()
	{
		m_currentPath.clear();

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		if( m_pSearchHandle != INVALID_HANDLE_VALUE )
		{
			FindClose( m_pSearchHandle );
			m_pSearchHandle = INVALID_HANDLE_VALUE;
		}
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
		if( m_pDir != nullptr )
		{
			closedir( m_pDir );
			m_pDir = nullptr;
		}
#endif
	}
}