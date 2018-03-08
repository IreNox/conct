#include "conct_runtime_high.h"

#include "conct_device.h"
#include "conct_port.h"
#include "conct_proxy.h"
#include "conct_reader.h"
#include "conct_runtime.h"
#include "conct_writer.h"

namespace conct
{
	void RuntimeHigh::setup( Device* pDevice )
	{
		pDevice->getInstances( m_instances );
	}

	void RuntimeHigh::processPort( Port* pPort )
	{
		Reader reader;
		if( !pPort->openReceived( reader, 0u ) )
		{
			return;
		}

// 		if( m_state == State_ExecuteCommand )
// 		{
// 			executeCommand();
// 		}
// 
// 		if( m_state == State_SendResponse )
// 		{
// 			sendData( pPort );
// 		}
// 		else
// 		{
// 			processData( pPort );
// 		}
	}
}