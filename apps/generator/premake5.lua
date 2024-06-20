
add_module_include_path( "../../modules" )
add_module_include_path( "../.." )

local project = Project:new( "generator", ProjectTypes.ConsoleApplication )

project:add_dependency( "conct_core_tool" )
project:add_dependency( "conct_types" )

project:add_files( 'include/*.h' )
project:add_files( 'sources/*.cpp' )

project:add_include_dir( "include" )

finalize_default_solution( project )
