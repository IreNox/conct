#include "conct_path.h"

namespace conct
{
	Path::Path()
	{

	}

	Path::Path( const DynamicString& path )
	{
		m_path = path.replace( '\\', '/' );
	}

	bool Path::isAbsolute()
	{
		return m_path[ 0u ] == '/' || m_path[ 1u ] == ':';
	}

	bool Path::isRelative()
	{
		return !isAbsolute();
	}

	Path Path::getParent() const
	{
		
	}

	Path Path::push( const Path& path ) const
	{

	}

	Path Path::push( const DynamicString& path ) const
	{

	}

	bool Path::operator!=( const Path& rhs ) const
	{

	}

	bool Path::operator==( const Path& rhs ) const
	{

	}

}
