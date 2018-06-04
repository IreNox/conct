#include "conct_controller.h"

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

	Command< RemoteInstance >* Controller::getInstance( const DeviceAddress& deviceAddress, TypeCrc typeCrc )
	{
		GetInstanceRequest request;
		request.typeCrc = typeCrc;

		ArrayView< uint8 > payload( (uint8*)&request, sizeof( request ) );
		return beginCommand< Command< RemoteInstance > >( deviceAddress, payload, MessageType_GetInstanceRequest );
	}

	Command< Array< RemoteInstance > >* Controller::findInstances( const DeviceAddress& deviceAddress, TypeCrc typeCrc )
	{
		FindInstancesRequest request;
		request.typeCrc = typeCrc;

		ArrayView< uint8 > payload( ( uint8* )&request, sizeof( request ) );
		return beginCommand< Command< Array< RemoteInstance > > >( deviceAddress, payload, MessageType_FindInstancesRequest );
	}

	Command< Value >* Controller::getProperty( const RemoteInstance& instance, const char* pName )
	{
		uint8 buffer[ 1024u ];
		GetPropertyRequest* pRequest = ( GetPropertyRequest* )buffer;
		pRequest->instanceId = instance.id;
		copyString( pRequest->name, sizeof( buffer ) - sizeof( *pRequest ), pName );

		ArrayView< uint8 > payload( buffer, sizeof( *pRequest ) + getStringLength( pName ) );
		return beginCommand< Command< Value > >( instance.address, payload, MessageType_GetPropertyRequest );
	}

	CommandBase* Controller::setProperty( const RemoteInstance& instance, const char* pName, const Value& value )
	{
		SetPropertyRequest request;
		request.instanceId	= instance.id;
		request.value		= value;

		ArrayView< uint8 > payload( ( uint8* )&request, sizeof( request ) );
		return beginCommand< CommandBase >( instance.address, payload, MessageType_SetPropertyRequest );
	}

	Command< Value >* Controller::callFunction( const RemoteInstance& instance, const char* pName, const ArrayView< Value >& arguments )
	{
		CallFunctionRequest request;
		request.instanceId = instance.id;

		ArrayView< uint8 > payload( ( uint8* )&request, sizeof( request ) );
		return beginCommand< Command< Value > >( instance.address, payload, MessageType_CallFunctionRequest );
	}

	template< class TCommand >
	TCommand* Controller::beginCommand( const DeviceAddress& deviceAddress, const ArrayView< uint8 >& payload, MessageType messageType )
	{
		CONCT_ASSERT( m_pRuntime != nullptr );

		const DeviceId deviceId = deviceAddress.address[ 0u ];
		const CommandId commandId = m_pRuntime->getNextCommandId( deviceId );
		if( commandId == InvalidCommandId )
		{
			return nullptr;
		}

		TCommand* pCommand = new TCommand( commandId );
		if( m_pRuntime->sendPackage( pCommand, deviceAddress, payload, messageType ) != ResultId_Success )
		{
			delete pCommand;
			pCommand = nullptr;
		}

		return pCommand;
	}
}
