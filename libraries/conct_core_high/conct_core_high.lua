local module = Module:new()

module:add_include_dir( "." )

module:add_files( "*.h" )
module:add_files( "*.cpp" )
module:add_files( "*.inl" )
module:add_files( "*.natvis" )

if tiki.target_platform == Platforms.Windows then
	module:set_define( "_CRT_SECURE_NO_WARNINGS" )
end

module:add_dependency( "conct_core" )
