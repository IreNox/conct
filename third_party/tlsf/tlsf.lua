local module = Module:new()
module.module_type = ModuleTypes.UnityCModule

module:add_include_dir( ".." )

module:add_files( "*.h" )
module:add_files( "*.c" )
