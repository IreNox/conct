return function( data, config )
	if not os.isfile( data.definition_path ) then
		throw("No device definition found at '" .. data.definition_path .. "'.");
	end

	local output_pattern = path.join( path.getdirectory( data.definition_path ), "gen/*.*" );
	local output_files = os.matchfiles( output_pattern );

	local needs_build = (#output_files == 0)
	for _, filename in ipairs( output_files ) do
		needs_build = is_build_required( data.definition_path, filename ) or needs_build;
	end
	
	if needs_build then
		local command_line = data.tool_path .. " " .. data.definition_path;
		
		print( "Generating Device Code..." );
		if not os.execute( command_line ) then
			error( "Generator returned an error. Command line: " .. command_line );
		end
	end
end