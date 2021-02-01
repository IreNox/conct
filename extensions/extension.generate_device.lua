
ProjectExtensions:add_new_hook( function( project )
	project.enable_generate_device = false
end )

ProjectExtensions:add_pre_finalize_hook( function( solution, project )
	if not project.enable_generate_device then
		return 		
	end
	
	local exe_name = iff( tiki.platform == Platforms.Windows, ".exe", "" )

	local step_data = {
		tool_path = path.join( tiki.root_path, "apps/generator/build/Release_x64/generator" .. exe_name ),
		project_path = project.module.config.base_path
	}
	
	project:add_pre_build_step( "generate_device", step_data )
	project:execute_build_step( "generate_device", step_data )

	project:add_files( "sources/gen/*.*" )
end )

function Project:generate_device()
	self.enable_generate_device = true
end
