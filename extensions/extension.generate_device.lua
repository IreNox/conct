
ProjectExtensions:add_new_hook( function( project )
	project.device_files = {}
end )

ProjectExtensions:add_pre_finalize_hook( function( solution, project )
	local exe_name = iff( tiki.platform == Platforms.Windows, ".exe", "" )

	for _, filename in pairs( project.device_files ) do
		local definition_path = path.join( project.module.config.base_path, filename )
	
		local step_data = {
			tool_path = path.join( tiki.root_path, "apps/generator/build/Release_x64/generator" .. exe_name ),
			definition_path = definition_path
		}
		
		project:add_pre_build_step( "generate_device", step_data )
		project:execute_build_step( "generate_device", step_data )

		local output_path = path.getdirectory( definition_path )
		project:add_files( output_path .. "/gen/*.*" )
	end
end )

function Project:add_device( filename )
	self.device_files[ #self.device_files ] = filename
end
