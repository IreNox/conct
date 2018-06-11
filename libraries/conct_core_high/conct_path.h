#pragma once

#include "conct_dynamic_string.h"

namespace conct
{
	class Path
	{
	public:

						Path();
		explicit		Path( const DynamicString& path );

		bool			isEmpty() { return m_path.isEmpty(); }
		bool			isAbsolute();
		bool			isRelative();

		Path			getParent() const;

		Path			push( const Path& path ) const;
		Path			push( const DynamicString& path ) const;

		DynamicString	getNativePath() const { return m_path; }
		DynamicString	getGenericPath() const { return m_path; }

		bool			operator==( const Path& rhs ) const;
		bool			operator!=( const Path& rhs ) const;

	private:

		DynamicString	m_path;
	};
}