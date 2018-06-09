#include "conct_controller.h"

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

	}

	void Controller::setup( RuntimeHigh* pRuntime )
	{
		m_pRuntime = pRuntime;
	}

	void Controller::loop()
	{
		for( uintreg i = 0u; i < m_releaseCommand.getCount(); ++i )
		{
			CommandBase* pCommand = m_releaseCommand[ i ];
			if( !pCommand->isFinish() )
			{
				continue;
			}

			delete pCommand;
			m_releaseCommand.eraseUnsortedByIndex( i );
			i--;
		}
	}

	Command< ValueHigh >* Controller::getProperty( const RemoteInstance& instance, const char* pName )
	{
		BufferedDataBuilder< 1024u > dataBuilder;
		GetPropertyRequest* pRequest = dataBuilder.pushStruct< GetPropertyRequest >();
		pRequest->instanceId	= instance.id;
		pRequest->name			= dataBuilder.pushString( pName );

		return beginCommand< Command< ValueHigh > >( instance.address, dataBuilder, MessageType_GetPropertyRequest );
	}

	CommandBase* Controller::setProperty( const RemoteInstance& instance, const char* pName, const ValueHigh& value )
	{
		BufferedDataBuilder< 1024u > dataBuilder;
		SetPropertyRequest* pRequest = dataBuilder.pushStruct< SetPropertyRequest >();
		pRequest->instanceId	= instance.id;
		pRequest->name			= dataBuilder.pushString( pName );

		dataBuilder.pushValueData( &pRequest->value, &value );

		return beginCommand< CommandBase >( instance.address, dataBuilder, MessageType_SetPropertyRequest );
	}

	Command< ValueHigh >* Controller::callFunction( const RemoteInstance& instance, const char* pName, const ArrayView< ValueHigh >& arguments )
	{
		BufferedDataBuilder< 1024u > dataBuilder;
		CallFunctionRequest* pRequest = dataBuilder.pushStruct< CallFunctionRequest >();
		pRequest->instanceId	= instance.id;
		pRequest->name			= dataBuilder.pushString( pName );

		Array< Value > workingArguments = dataBuilder.pushArray< Value >( arguments.getCount() );
		pRequest->arguments		= workingArguments;

		for( size_t i = 0u; i < workingArguments.getCount(); ++i )
		{
			dataBuilder.pushValueData( &workingArguments[ i ], &arguments[ i ] );
		}

		return beginCommand< Command< ValueHigh > >( instance.address, dataBuilder, MessageType_CallFunctionRequest );
	}

	void Controller::releaseCommand( CommandBase* pCommand )
	{
		m_releaseCommand.pushBack( pCommand );
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
		if( m_pRuntime->sendPackage( pCommand, deviceAddress, payload.toArrayView(), messageType ) != ResultId_Success )
		{
			delete pCommand;
			pCommand = nullptr;
		}

		return pCommand;
	}
}
