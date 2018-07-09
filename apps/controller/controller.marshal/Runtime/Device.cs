using System;
using System.Collections.Generic;

namespace conct
{
	public class Device : IDisposable
	{
		private IntPtr m_nativeInstance;

		private Controller m_controller;

		public Device()
		{
			m_nativeInstance = DeviceNative.Create();

			IntPtr controllerHandle = DeviceNative.GetController(m_nativeInstance);
			m_controller = new Controller(controllerHandle);
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

		public void Loop()
		{
			DeviceNative.Loop(m_nativeInstance);
		}

		public IntPtr AddPort(string hostname, UInt16 hostPort)
		{
			return DeviceNative.AddPort(m_nativeInstance, hostname, hostPort);
		}

		public void RemovePort(IntPtr portHandle)
		{
			DeviceNative.RemovePort(m_nativeInstance, portHandle);
		}

		public bool IsThisDevice(DeviceAddress address)
		{
			return DeviceNative.IsThisDevice(m_nativeInstance, address.NativeInstance);
		}

		public Controller Controller
		{
			get
			{
				return m_controller;
			}
		}

		public List<DeviceAddress> ConnectedDevices
		{
			get
			{
				List<DeviceAddress> devices = new List<DeviceAddress>();

				int count = DeviceNative.GetConnectedDeviceCount(m_nativeInstance);
				for (int i = 0; i < count; i++)
				{
					IntPtr addressHandle = DeviceNative.GetConnectedDevice(m_nativeInstance, i);
					devices.Add(new DeviceAddress(addressHandle));
				}

				return devices;
			}
		}
	}
}