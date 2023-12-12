local project = Project:new( "generator", ProjectTypes.ConsoleApplication )

project:add_dependency( "conct_core" )
project:add_dependency( "conct_core_high" )
project:add_dependency( "conct_core_tool" )
project:add_dependency( "conct_types" )

project:add_dependency( "third_party/tinyxml2" )
project:add_dependency( "third_party/tlsf" )

project:add_files( 'include/*.h' )
project:add_files( 'sources/*.cpp' )

project:add_include_dir( "include" )

add_module_include_path( "../../libraries" )
add_module_include_path( "../.." )
finalize_default_solution( project )
