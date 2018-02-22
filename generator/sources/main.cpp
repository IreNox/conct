#include "generator.h"

int main( int argc, char* argv[] )
{
	std::vector< std::string > arguments;
	for( int i = 0u; i < argc; ++i )
	{
		arguments.push_back( argv[ i ] );
	}

	conct::Generator generator;
	const bool result = generator.run( arguments );
    return result ? 0 : -1;
}

