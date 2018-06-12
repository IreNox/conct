#include "conct_filesystem.h"

#include "conct_dynamic_string.h"
#include "conct_path.h"
#include "conct_vector.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	include <windows.h>
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
#	include <unistd.h>
#	include <sys/stat.h>
#endif

#include <stdio.h>
#include <cerrno>

namespace conct
{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
	ResultId getResultFromLastError()
	{
		const DWORD lastError = GetLastError();
		if( lastError == ERROR_ALREADY_EXISTS )
		{
			return ResultId_AlreadyExists;
		}
		else if( lastError == ERROR_PATH_NOT_FOUND )
		{
			return ResultId_PathNotFound;
		}

		return ResultId_Unknown;
	}
#endif

	ResultId getResultFromErrno()
	{
		const int errorCode = errno;

		switch( errorCode )
		{
		case ENOENT:
			return ResultId_PathNotFound;

		case EACCES:
		case EROFS:
			return ResultId_PermissionDenied;

		case EEXIST:
			return ResultId_AlreadyExists;

		default:
			break;
		}

		return ResultId_Unknown;
	}

	bool filesystem::exists( const Path& path )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		return ( GetFileAttributesA( path.getNativePath().toConstCharPointer() ) != INVALID_FILE_ATTRIBUTES );
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
		return access( path.getNativePath().toConstCharPointer(), F_OK ) != -1;
#endif
	}

	bool filesystem::isFile( const Path& path )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		const DWORD attributes = GetFileAttributesA( path.getNativePath().toConstCharPointer() );
		if( attributes == INVALID_FILE_ATTRIBUTES )
		{
			return false;
		}

		return !isBitSet( attributes, FILE_ATTRIBUTE_DIRECTORY );
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
		struct stat fileStats;
		if( ::stat( path.getNativePath().toConstCharPointer(), &fileStats ) != 0 )
		{
			return false;
		}

		return isBitSet( fileStats.st_mode, S_IFMT );
#endif
	}

	bool filesystem::isDirectory( const Path& path )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		return isBitSet( GetFileAttributesA( path.getNativePath().toConstCharPointer() ), FILE_ATTRIBUTE_DIRECTORY );
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
		struct stat fileStats;
		if( ::stat( path.getNativePath().toConstCharPointer(), &fileStats ) != 0 )
		{
			return false;
		}

		return S_ISDIR( fileStats.st_mode );
#endif
	}

	Result< void > filesystem::createDirectory( const Path& path )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		if( !CreateDirectoryA( path.getNativePath().toConstCharPointer(), nullptr ) )
		{
			return createFailureResult< void >( getResultFromLastError() );
		}

		return createSuccessResult();
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
		if( mkdir( path.getNativePath().toConstCharPointer(), S_IRWXU ) != 0 )
		{
			return createFailureResult< void >( getResultFromErrno() );
		}

		return createSuccessResult();
#endif
	}

	Result< DynamicString > filesystem::readTextFile( const Path& path )
	{
		FILE* pFile = fopen( path.getNativePath().toConstCharPointer(), "rb" );
		if( pFile == nullptr )
		{
			return createFailureResult< DynamicString >( getResultFromErrno() );
		}

		fpos_t pos;
		fseek( pFile, 0, SEEK_END );
		fgetpos( pFile, &pos );
		fseek( pFile, 0, SEEK_SET );

		DynamicString result;
		result.reserve( uintreg( pos ) );

		if( fread( result.toCharPointer(), 1u, pos, pFile ) != pos )
		{
			fclose( pFile );
			return createFailureResult< DynamicString >( getResultFromErrno() );
		}
		fclose( pFile );

		result.terminate( uintreg( pos ) );

		return createSuccessResult( result );
	}

	Result< Vector< uint8 > > filesystem::readBinaryFile( const Path& path )
	{
		FILE* pFile = fopen( path.getNativePath().toConstCharPointer(), "rb" );
		if( pFile == nullptr )
		{
			return createFailureResult< Vector< uint8 > >( getResultFromErrno() );
		}

		fpos_t pos;
		fseek( pFile, 0, SEEK_END );
		fgetpos( pFile, &pos );
		fseek( pFile, 0, SEEK_SET );

		Vector< uint8 > result;
		result.setLength( uintreg( pos ) );

		if( fread( result.getData(), 1u, pos, pFile ) != pos )
		{
			fclose( pFile );
			return createFailureResult< Vector< uint8 > >( getResultFromErrno() );
		}
		fclose( pFile );

		return createSuccessResult( result );
	}

	Result< void > filesystem::writeTextFile( const Path& path, const DynamicString& text )
	{
		FILE* pFile = fopen( path.getNativePath().toConstCharPointer(), "wb" );
		if( pFile == nullptr )
		{
			return createFailureResult< void >( getResultFromErrno() );
		}

		if( fwrite( text.toConstCharPointer(), 1u, text.getLength(), pFile ) != text.getLength() )
		{
			fclose( pFile );
			return createFailureResult< void >( getResultFromErrno() );
		}

		fclose( pFile );
		return createSuccessResult();
	}

	Result< void > filesystem::writeBinaryFile( const Path& path, const Vector< uint8 >& data )
	{
		FILE* pFile = fopen( path.getNativePath().toConstCharPointer(), "wb" );
		if( pFile == nullptr )
		{
			return createFailureResult< void >( getResultFromErrno() );
		}

		if( fwrite( data.getData(), 1u, data.getLength(), pFile ) != data.getLength() )
		{
			fclose( pFile );
			return createFailureResult< void >( getResultFromErrno() );
		}

		fclose( pFile );
		return createSuccessResult();
	}
}
