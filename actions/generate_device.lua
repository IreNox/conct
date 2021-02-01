return function( data, config )
	local definition_path = path.join( data.project_path, "sources/device.xml" );
	if not os.isfile( definition_path ) then
		throw("No device definition found at '" .. definition_path .. "'.");
	end

	local output_pattern = path.join( config.project_path, "sources/gen/*.*" );
	local output_files = os.matchfiles( output_pattern );

	local needs_build = (#output_files == 0)
	for i, filename in pairs( output_files ) do
		needs_build = is_build_required( definition_path, filename ) or needs_build;
	end
	
	if needs_build then
		local command_line = data.tool_path .. " " .. definition_path;
		
		print( "Generating Device Code..." );
		if not os.execute( command_line ) then
			error( "Generator returned an error. Command line: " .. command_line );
		end
	end
end