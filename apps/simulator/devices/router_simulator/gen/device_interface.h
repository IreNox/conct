#pragma once

#include "conct_device.h"
#include "conct_runtime.h"

#include "conct_port_tcp_server.h"

#include "core_device_proxy.h"
#include "system_clipboard_proxy.h"
#include "system_browser_proxy.h"
#include "core_router_proxy.h"

#include "../system_clipboard_impl.h"
#include "../system_browser_impl.h"
#include "conct_router.h"

namespace conct
{
	class DeviceInterface : public Device
	{
	public:

		DeviceInterface();

		void setupDevice();
		void loopDevice();

		virtual const char* getName() const override final;

	protected:

		Runtime m_runtime;

		PortTcpServer m_port0;

		DeviceProxy m_proxyDevice;
		ClipboardProxy m_proxyClipboard;
		BrowserProxy m_proxyBrowser;
		RouterProxy m_proxyRouter;

		Router m_instanceRouter;
		BrowserImpl m_instanceBrowser;
		ClipboardImpl m_instanceClipboard;

		virtual void setup() = 0;
		virtual void loop() = 0;

		virtual void getEmptyInstances( ArrayView< Instance >& instances ) override final;
		virtual void getPublicInstances( ConstInstanceView& instances ) const override final;
		virtual void getLocalInstances( ArrayView< const LocalInstance >& instances ) override final;
	};
}
