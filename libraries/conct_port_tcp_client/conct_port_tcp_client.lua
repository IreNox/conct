local module = Module:new()

module:add_include_dir( "." )

module:add_files( "*.h" )
module:add_files( "*.cpp" )

module:add_library_file( "ws2_32" )
