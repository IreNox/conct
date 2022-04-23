local module = Module:new()

module:add_include_dir( "." )

module:add_files( "*.h" )
module:add_files( "*.cpp" )

if tiki.is_simulator then
	module:add_include_dir( "simulator" )
	
	module:add_files( "simulator/*.h" )
	module:add_files( "simulator/*.cpp" )
end
