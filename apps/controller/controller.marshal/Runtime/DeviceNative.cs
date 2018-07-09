using System;
using System.Runtime.InteropServices;

namespace conct
{
	internal static class DeviceNative
	{
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_device_create")]
		public static extern IntPtr Create();
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_device_destroy")]
		public static extern void Destroy(IntPtr handle);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_device_loop")]
		public static extern void Loop(IntPtr handle);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_device_add_port")]
		public static extern IntPtr AddPort(IntPtr handle, [param: MarshalAs(UnmanagedType.LPStr)] string hostname, UInt16 hostPort);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_device_remove_port")]
		public static extern void RemovePort(IntPtr handle, IntPtr portHandle);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_device_get_controller")]
		public static extern IntPtr GetController(IntPtr deviceHandle);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_device_get_connected_device_count")]
		public static extern int GetConnectedDeviceCount(IntPtr deviceHandle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_device_get_connected_device")]
		public static extern IntPtr GetConnectedDevice(IntPtr deviceHandle, int index);
	}
}