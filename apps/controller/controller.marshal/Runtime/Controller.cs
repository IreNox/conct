using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System;

namespace conct
{
	public class Controller
	{
		private IntPtr m_nativeInstance;

		internal Controller(IntPtr nativeInstance)
		{
			m_nativeInstance = nativeInstance;
		}

		public Command GetProperty(DeviceAddress deviceAddress, UInt16 instanceId, string name)
		{
			IntPtr commandHandle = ControllerNative.GetProperty(m_nativeInstance, deviceAddress.NativeInstance, instanceId, name);
			if (commandHandle == null)
			{
				return null;
			}

			return new Command(commandHandle, true);
		}

		public Command SetProperty(DeviceAddress deviceAddress, UInt16 instanceId, string name, Value value)
		{
			IntPtr commandHandle = ControllerNative.SetProperty(m_nativeInstance, deviceAddress.NativeInstance, instanceId, name, value.NativeInstance);
			if (commandHandle == null)
			{
				return null;
			}

			return new Command(commandHandle, false);
		}

		public Command CallFunction(DeviceAddress deviceAddress, UInt16 instanceId, string name, Value[] values)
		{
			IntPtr nativeValues = Marshal.AllocHGlobal(Marshal.SizeOf<IntPtr>() * values.Length);

			IntPtr currentElement = nativeValues;
			for (int i = 0; i < values.Length; i++)
			{
				Marshal.WriteIntPtr(currentElement, values[i].NativeInstance);
				currentElement += Marshal.SizeOf<IntPtr>();
			}

			IntPtr commandHandle = ControllerNative.CallFunction(m_nativeInstance, deviceAddress.NativeInstance, instanceId, name, nativeValues, values.Length);

			Marshal.FreeHGlobal(nativeValues);
			if (commandHandle == null)
			{
				return null;
			}

			return new Command(commandHandle, true);
		}
	}
}