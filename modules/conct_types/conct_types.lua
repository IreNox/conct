local module = Module:new()

module:add_include_dir( "." )

module:add_files( "*.h" )
module:add_files( "*.cpp" )
module:add_files( "*.inl" )

module:add_dependency( "third_party/tinyxml2" )
