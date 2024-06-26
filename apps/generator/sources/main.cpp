#include "generator.h"

#include "conct_dynamic_string.h"

int main( int argc, char* argv[] )
{
	conct::Vector< conct::DynamicString > arguments;
	for( int i = 0u; i < argc; ++i )
	{
		arguments.pushBack( conct::DynamicString( argv[ i ] ) );
	}

	conct::Generator generator;
	const bool result = generator.run( arguments );
    return result ? 0 : -1;
}
