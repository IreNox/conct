using System;

namespace conct
{
	public class DeviceAddress
	{
		private IntPtr m_nativeInstance;

		internal DeviceAddress(IntPtr nativeInstance)
		{
			m_nativeInstance = nativeInstance;
		}

		internal IntPtr NativeInstance
		{
			get { return m_nativeInstance; }
		}
	}
}