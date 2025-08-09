tiki.add_extension( "generate_device" );

local project = Project:new( "service", ProjectTypes.ConsoleApplication )

project:add_dependency( "conct_core" )
project:add_dependency( "conct_core_tool" )
project:add_dependency( "conct_port_tcp_client" )
project:add_dependency( "conct_runtime" )

project:add_files( 'sources/*.h' )
project:add_files( 'sources/*.cpp' )

project:add_device( "sources/device.xml" )

add_module_include_path( "../../modules" )
add_module_include_path( "../.." )
finalize_default_solution( project )
