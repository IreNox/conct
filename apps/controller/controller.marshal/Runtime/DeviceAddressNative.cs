using System;
using System.Runtime.InteropServices;

namespace conct
{
	internal static class DeviceAddressNative
	{
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_device_address_create")]
		public static extern IntPtr Create();
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_device_address_duplicate")]
		public static extern IntPtr Duplicate(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_device_address_destroy")]
		public static extern void Destroy(IntPtr handle);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_device_address_get_device_count")]
		public static extern int GetDeviceCount(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_device_address_get_device_id")]
		public static extern byte GetDevice(IntPtr handle, int index);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_device_address_push_device")]
		public static extern void Push(IntPtr handle, byte deviceId);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_device_address_pop_device")]
		public static extern byte Pop(IntPtr handle);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_device_address_is_empty")]
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool IsEmpty(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_device_address_is_equals")]
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool IsEquals(IntPtr handle1, IntPtr handle2);
	}
}