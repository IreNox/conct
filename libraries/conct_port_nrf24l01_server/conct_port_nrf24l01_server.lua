local module = Module:new()

module:add_include_dir( "." )

module:add_files( "*.h" )
module:add_files( "*.cpp" )

module:add_dependency( "conct_port_nrf24l01" )
