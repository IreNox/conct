#include "generator.h"

#include "tiki/tiki_dynamic_string.h"

int main( int argc, char* argv[] )
{
	tiki::DynamicArray< tiki::DynamicString > arguments;
	for( int i = 0u; i < argc; ++i )
	{
		arguments.pushBack( tiki::DynamicString( argv[ i ] ) );
	}

	conct::Generator generator;
	const bool result = generator.run( arguments );
    return result ? 0 : -1;
}
