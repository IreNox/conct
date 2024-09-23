#include "conct_controller.h"

#include "conct_crc16.h"
#include "conct_runtime.h"

namespace conct
{
	Controller::Controller()
		: m_pRuntime( nullptr )
	{
	}

	Controller::~Controller()
	{
		for( uintreg i = 0u; i < m_releaseCommand.getLength(); ++i )
		{
			delete m_releaseCommand[ i ];
		}
	}

	void Controller::setup( Runtime& runtime )
	{
		m_pRuntime = &runtime;
	}

	void Controller::loop()
	{
		Command* pCommand = nullptr;
		while( m_pRuntime->popFinishCommand( pCommand ) )
		{
			for( uintreg i = 0u; i < m_callbacks.getLength(); ++i )
			{
				m_callbacks[ i ]( pCommand );
			}
		}

		for( uintreg i = 0u; i < m_releaseCommand.getLength(); ++i )
		{
			pCommand = m_releaseCommand[ i ];
			if( !pCommand->isFinish() )
			{
				continue;
			}

			delete pCommand;
			m_releaseCommand.eraseUnsortedByIndex( i );
			i--;
		}
	}

	ValueCommand* Controller::getProperty( const RemoteInstance& instance, const char* pName )
	{
		GetPropertyRequest request;
		request.instanceId	= instance.id;
		request.nameCrc		= calculateStringCrc16( pName );

		const ConstArrayView< byte > payload( (const byte*)&request, sizeof(request));
		return beginCommand< ValueCommand >( instance.address, payload, MessageType_GetPropertyRequest );
	}

	Command* Controller::setProperty( const RemoteInstance& instance, const char* pName, const Value& value )
	{
		DynamicArray< byte > payload;

		SetPropertyRequest* pRequest = (SetPropertyRequest*)payload.pushRange( sizeof( SetPropertyRequest ) ).getData();
		pRequest->instanceId	= instance.id;
		pRequest->nameCrc		= calculateStringCrc16( pName );

		value.serialize( payload );

		return beginCommand< Command >( instance.address, payload, MessageType_SetPropertyRequest );
	}

	ValueCommand* Controller::callFunction( const RemoteInstance& instance, const char* pName, const ConstArrayView< Value >& arguments )
	{
		DynamicArray< byte > payload;

		CallFunctionRequest* pRequest = (CallFunctionRequest*)payload.pushRange( sizeof( CallFunctionRequest ) ).getData();
		pRequest->instanceId	= instance.id;
		pRequest->nameCrc		= calculateStringCrc16( pName );
		pRequest->argumentCount	= (uint16)arguments.getLength();

		for( uintreg i = 0u; i < arguments.getLength(); ++i )
		{
			arguments[ i ].serialize( payload );
		}

		return beginCommand< ValueCommand >( instance.address, payload, MessageType_CallFunctionRequest );
	}

	void Controller::releaseCommand( Command* pCommand )
	{
		m_releaseCommand.pushBack( pCommand );
	}

	void Controller::registerCommandCallback( ControllerCommandCallback callback )
	{
		m_callbacks.pushBack( callback );
	}

	void Controller::unregisterCommandCallback( ControllerCommandCallback callback )
	{
		for( uintreg i = 0u; i < m_callbacks.getLength(); ++i )
		{
			if( m_callbacks[ i ] == callback )
			{
				m_callbacks.eraseUnsortedByIndex( i );
				return;
			}
		}
	}

	template< class TCommand >
	TCommand* Controller::beginCommand( const DeviceAddress& deviceAddress, const ConstArrayView< byte >& payload, MessageType messageType )
	{
		TIKI_ASSERT( m_pRuntime != nullptr );

		const DeviceId deviceId = deviceAddress.address[ 0u ];
		const CommandId commandId = m_pRuntime->getNextCommandId( deviceId );
		if( commandId == InvalidCommandId )
		{
			return nullptr;
		}

		TCommand* pCommand = new TCommand( commandId );
		if( m_pRuntime->sendCommandPackage( pCommand, deviceAddress, payload, messageType ) != ResultId_Success )
		{
			delete pCommand;
			pCommand = nullptr;
		}

		return pCommand;
	}
}
