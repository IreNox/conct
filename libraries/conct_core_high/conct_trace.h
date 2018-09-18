#pragma once

namespace conct
{
	class DynamicString;

	namespace trace
	{
		void	write( const char* pString );
		void	write( const DynamicString& string );

		void	fileError( const char* pFilename, int line, const DynamicString& string );
		void	fileError( const DynamicString& filename, int line, const DynamicString& string );
	}
}
