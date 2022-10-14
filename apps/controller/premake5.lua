tiki.add_extension( "generate_device" );

local project = Project:new( "controller", ProjectTypes.WindowApplication )

project:add_dependency( "conct_core" )
project:add_dependency( "conct_core_high" )
project:add_dependency( "conct_core_tool" )
project:add_dependency( "conct_port_tcp_client" )
project:add_dependency( "conct_runtime" )
project:add_dependency( "conct_runtime_high" )
project:add_dependency( "conct_types" )

project:add_dependency( "third_party/tinyxml2" )
project:add_dependency( "third_party/tlsf" )

project:add_files( 'sources/*.h' )
project:add_files( 'sources/*.cpp' )

project:add_external( "https://github.com/irenox/imapp.git" )

project:add_post_build_step( "copy_files", { pattern = "assets/**" } )

project:add_device( "sources/device.xml" )

add_module_include_path( "../../libraries" )
add_module_include_path( "../.." )
finalize_default_solution( project )
