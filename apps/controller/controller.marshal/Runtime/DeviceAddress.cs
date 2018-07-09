using System;

namespace conct
{
	public class DeviceAddress : IDisposable
	{
		private IntPtr m_nativeInstance;

		public DeviceAddress()
		{
			m_nativeInstance = DeviceAddressNative.Create();
		}

		public void Dispose()
		{
			if (m_nativeInstance != IntPtr.Zero)
			{
				DeviceAddressNative.Destroy(m_nativeInstance);
				m_nativeInstance = IntPtr.Zero;
			}
		}

		internal DeviceAddress(IntPtr nativeInstance)
		{
			m_nativeInstance = nativeInstance;
		}

		internal IntPtr NativeInstance
		{
			get { return m_nativeInstance; }
		}

		public bool Empty
		{
			get { return DeviceAddressNative.IsEmpty(m_nativeInstance); }
		}

		public string Text
		{
			get
			{
				int count = DeviceAddressNative.GetDeviceCount(m_nativeInstance);

				byte[] devices = new byte[count];
				for (int i = 0; i < count; i++)
				{
					devices[i] = DeviceAddressNative.GetDevice(m_nativeInstance, i);
				}

				return BitConverter.ToString(devices);
			}
		}

		public override bool Equals(object obj)
		{
			DeviceAddress address = obj as DeviceAddress;
			if (address == null)
			{
				return false;
			}

			return this == address;
		}

		public override int GetHashCode()
		{
			// TODO?
			return base.GetHashCode();
		}

		public static bool operator ==(DeviceAddress lhs, DeviceAddress rhs)
		{
			return DeviceAddressNative.IsEquals(lhs.m_nativeInstance, rhs.m_nativeInstance);
		}

		public static bool operator !=(DeviceAddress lhs, DeviceAddress rhs)
		{
			return !DeviceAddressNative.IsEquals(lhs.m_nativeInstance, rhs.m_nativeInstance);
		}
	}
}