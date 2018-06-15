#pragma once

namespace conct
{
	class DynamicString;

	namespace trace
	{
		void	write( const char* pString );
		void	write( const DynamicString& string );
	}
}
