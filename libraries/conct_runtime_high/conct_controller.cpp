#include "conct_controller.h"

#include "conct_crc16.h"
#include "conct_data_builder.h"
#include "conct_runtime_high.h"
#include "conct_string.h"

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

	void Controller::setup( RuntimeHigh& runtime )
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
		BufferedDataBuilder< 1024u > dataBuilder;
		GetPropertyRequest* pRequest = dataBuilder.pushStruct< GetPropertyRequest >();
		pRequest->instanceId	= instance.id;
		pRequest->nameCrc		= calculateStringCrc16( pName );

		return beginCommand< ValueCommand >( instance.address, dataBuilder, MessageType_GetPropertyRequest );
	}

	Command* Controller::setProperty( const RemoteInstance& instance, const char* pName, const ValueHigh& value )
	{
		BufferedDataBuilder< 1024u > dataBuilder;
		SetPropertyRequest* pRequest = dataBuilder.pushStruct< SetPropertyRequest >();
		pRequest->instanceId	= instance.id;
		pRequest->nameCrc		= calculateStringCrc16( pName );

		dataBuilder.pushValueData( &pRequest->value, &value );

		return beginCommand< Command >( instance.address, dataBuilder, MessageType_SetPropertyRequest );
	}

	ValueCommand* Controller::callFunction( const RemoteInstance& instance, const char* pName, const ArrayView< ValueHigh >& arguments )
	{
		BufferedDataBuilder< 1024u > dataBuilder;
		CallFunctionRequest* pRequest = dataBuilder.pushStruct< CallFunctionRequest >();
		pRequest->instanceId	= instance.id;
		pRequest->nameCrc		= calculateStringCrc16( pName );

		Array< Value > workingArguments = dataBuilder.pushArray< Value >( arguments.getLength() );
		pRequest->arguments		= workingArguments;

		for( uintreg i = 0u; i < workingArguments.getLength(); ++i )
		{
			dataBuilder.pushValueData( &workingArguments[ i ], &arguments[ i ] );
		}

		return beginCommand< ValueCommand >( instance.address, dataBuilder, MessageType_CallFunctionRequest );
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
	TCommand* Controller::beginCommand( const DeviceAddress& deviceAddress, const DataBuilder& payload, MessageType messageType )
	{
		CONCT_ASSERT( m_pRuntime != nullptr );

		if( payload.isExceeded() )
		{
			return nullptr;
		}

		const DeviceId deviceId = deviceAddress.address[ 0u ];
		const CommandId commandId = m_pRuntime->getNextCommandId( deviceId );
		if( commandId == InvalidCommandId )
		{
			return nullptr;
		}

		TCommand* pCommand = new TCommand( commandId );
		if( m_pRuntime->sendCommandPackage( pCommand, deviceAddress, payload.toArrayView(), messageType ) != ResultId_Success )
		{
			delete pCommand;
			pCommand = nullptr;
		}

		return pCommand;
	}
}
