using System;
using System.Runtime.InteropServices;

namespace conct
{
	internal static class ControllerNative
	{
		[DllImport("libconct", EntryPoint = "conct_controller_get_property")]
		public static extern IntPtr GetProperty(IntPtr handle, IntPtr addressHandle, UInt16 instanceId, [param: MarshalAs(UnmanagedType.LPUTF8Str)] string name);
		[DllImport("libconct", EntryPoint = "conct_controller_set_property")]
		public static extern IntPtr SetProperty(IntPtr handle, IntPtr addressHandle, UInt16 instanceId, [param: MarshalAs(UnmanagedType.LPUTF8Str)] string name, IntPtr valueHandle);
		[DllImport("libconct", EntryPoint = "conct_controller_call_function")]
		public static extern IntPtr CallFunction(IntPtr handle, IntPtr addressHandle, UInt16 instanceId, [param: MarshalAs(UnmanagedType.LPUTF8Str)] string name, IntPtr values, int valueCount);

		[DllImport("libconct", EntryPoint = "conct_controller_release_command")]
		public static extern IntPtr ReleaseCommand(IntPtr handle, IntPtr commandHandle);
	}
}