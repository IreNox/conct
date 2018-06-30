using System;
using System.Runtime.InteropServices;

namespace conct
{
	internal static class DeviceNative
	{
		[DllImport("libconct", EntryPoint = "conct_device_create")]
		public static extern IntPtr Create();
		[DllImport("libconct", EntryPoint = "conct_device_destroy")]
		public static extern void Destroy(IntPtr handle);

		[DllImport("libconct", EntryPoint = "conct_device_loop")]
		public static extern void Loop(IntPtr handle);

		[DllImport("libconct", EntryPoint = "conct_device_add_port")]
		public static extern IntPtr AddPort(IntPtr handle, [param: MarshalAs(UnmanagedType.LPUTF8Str)] string hostname, UInt16 hostPort);
		[DllImport("libconct", EntryPoint = "conct_device_remove_port")]
		public static extern void RemovePort(IntPtr handle, IntPtr portHandle);

		[DllImport("libconct", EntryPoint = "conct_device_get_controller")]
		public static extern IntPtr GetController(IntPtr deviceHandle);
	}
}