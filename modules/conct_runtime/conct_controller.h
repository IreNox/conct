#pragma once

#include "conct_command.h"
#include "conct_value_high.h"

#include <tiki/tiki_dynamic_array.h>

namespace conct
{
	class DataBuilder;
	class Runtime;

	typedef void( TIKI_CDECL *ControllerCommandCallback )( Command* pCommand );

	class Controller
	{
	public:

									Controller();
									~Controller();

		void						setup( Runtime& runtime );
		void						loop();

		ValueCommand*				getProperty( const RemoteInstance& instance, const char* pName );
		Command*					setProperty( const RemoteInstance& instance, const char* pName, const ValueHigh& value );

		ValueCommand*				callFunction( const RemoteInstance& instance, const char* pName, const ArrayView< ValueHigh >& arguments );

		void						releaseCommand( Command* pCommand );

		void						registerCommandCallback( ControllerCommandCallback callback );
		void						unregisterCommandCallback( ControllerCommandCallback callback );

	private:

		using CommandCallbackArray = DynamicArray< ControllerCommandCallback >;

		Runtime*					m_pRuntime;

		CommandCallbackArray		m_callbacks;
		DynamicArray< Command* >	m_releaseCommand;

		template< class TCommand >
		TCommand*					beginCommand( const DeviceAddress& deviceAddress, const DataBuilder& payload, MessageType messageType );
	};
}
