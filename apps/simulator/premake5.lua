tiki.add_extension( "generate_device" );

tiki.is_simulator = true

add_module_include_path( "../../modules" )
add_module_include_path( "../.." )

local main_project = Project:new( "simulator", ProjectTypes.ConsoleApplication )
main_project:set_setting( ConfigurationSettings.MultiProcessorCompile, ConfigurationMultiProcessorCompile.On )
main_project:add_include_dir( "include" )
main_project:add_include_dir( "devices/shared" )
main_project:add_dependency( "conct_runtime" )
main_project:add_dependency( "conct_simulator" )
main_project:add_dependency( "conct_types" )
main_project:add_files( 'sources/*.h' )
main_project:add_files( 'sources/*.cpp' )

function setup_simulator_device( project )
	project:set_base_path( project.module.config.base_path .. "/devices/" .. project.name )

	project:set_setting( ConfigurationSettings.MultiProcessorCompile, ConfigurationMultiProcessorCompile.On )

	project:add_include_dir( "../shared" )

	project:add_dependency( "conct_runtime" )
	project:add_dependency( "conct_simulator" )

	project:set_define( "CONCT_ENVIRONMENT_SIMULATOR", "TIKI_ON" )

	project:add_files( '*.h' )
	project:add_files( '*.cpp' )
	
	if tiki.target_platform == Platforms.Windows then
		project:set_define( "_WINDLL" )
		project:add_library_file( "simulator.lib" )
		project:add_library_dir( "$(OutDir)" )
	end

	project:add_device( "device.xml" );
	
	project:add_project_dependency( main_project )
end

local controller_project = Project:new( "controller_simulator", ProjectTypes.SharedLibrary )
setup_simulator_device( controller_project )
controller_project:add_dependency( "conct_port_tcp_client" )

local light_project = Project:new( "light_simulator", ProjectTypes.SharedLibrary )
setup_simulator_device( light_project )
light_project:add_dependency( "conct_port_tcp_client" )
light_project:add_dependency( "conct_light" )

local router_project = Project:new( "router_simulator", ProjectTypes.SharedLibrary )
setup_simulator_device( router_project )
router_project:add_dependency( "conct_port_tcp_client" )
router_project:add_dependency( "conct_port_tcp_server" )

finalize_default_solution( main_project, controller_project, light_project, router_project )
