#pragma once

#include "conct_dynamic_string.h"

namespace conct
{
	class Path
	{
	public:

						Path();
		explicit		Path( const DynamicString& path );

		bool			isEmpty() const { return m_path.isEmpty(); }
		bool			isAbsolute() const;
		bool			isRelative() const;

		void			clear();

		Path			getParent() const;

		Path			push( const Path& path ) const;
		Path			push( const DynamicString& pathString ) const;

		DynamicString	getFilename() const;
		DynamicString	getBasename() const;
		DynamicString	getExtension() const;

		DynamicString	getNativePath() const;
		DynamicString	getGenericPath() const { return m_path; }

		bool			operator==( const Path& rhs ) const;
		bool			operator!=( const Path& rhs ) const;

		static Path		getExecutablePath();

	private:

		DynamicString	m_path;
	};
}