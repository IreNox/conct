#pragma once

#include "conct_core.h"

#if CONCT_ENABLED( CONCT_ENVIRONMENT_TESTS )

#include "conct_mutex.h"
#include "conct_queue.h"

namespace conct
{
	class SerialTestContext
	{
	public:

		virtual bool				getGpio(uintreg pin) const;
		virtual void				setGpio(uintreg pin, bool value);

		virtual int					getAvailableSerialData() const;
		virtual uintreg				readSerialData(void* pBuffer, uintreg bufferLength);
		virtual void				writeSerialData(uint8 byte);

		static SerialTestContext&	get();

	private:

		Mutex						m_mutex;

	};
}

#endif