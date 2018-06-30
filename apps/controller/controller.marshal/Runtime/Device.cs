using System;

namespace conct
{
	public class Device : IDisposable
	{
		private IntPtr m_nativeInstance;

		private Controller m_controller;

		public Device()
		{
			m_nativeInstance = DeviceNative.Create();
		}

		public void Dispose()
		{
			m_controller = null;

			if (m_nativeInstance != IntPtr.Zero)
			{
				DeviceNative.Destroy(m_nativeInstance);
				m_nativeInstance = IntPtr.Zero;
			}
		}

		public IntPtr AddPort(string hostname, UInt16 hostPort)
		{
			return DeviceNative.AddPort(m_nativeInstance, hostname, hostPort);
		}

		public void RemovePort(IntPtr portHandle)
		{
			DeviceNative.RemovePort(m_nativeInstance, portHandle);
		}

		public Controller Controller
		{
			get
			{
				if (m_controller == null)
				{
					IntPtr controllerHandle = DeviceNative.GetController(m_nativeInstance);
					m_controller = new Controller(controllerHandle);
				}

				return m_controller;
			}
		}
	}
}