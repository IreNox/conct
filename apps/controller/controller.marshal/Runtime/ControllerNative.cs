using System;
using System.Runtime.InteropServices;

namespace conct
{
	internal static class ControllerNative
	{
		public delegate void CommandChangedDelegate(IntPtr commandHandle);

		[DllImport(DynamicLibrary.Name, CharSet = CharSet.Ansi, EntryPoint = "conct_controller_get_property")]
		public static extern IntPtr GetProperty(IntPtr handle, IntPtr addressHandle, UInt16 instanceId, [param: MarshalAs(UnmanagedType.LPStr)] string name);
		[DllImport(DynamicLibrary.Name, CharSet = CharSet.Ansi, EntryPoint = "conct_controller_set_property")]
		public static extern IntPtr SetProperty(IntPtr handle, IntPtr addressHandle, UInt16 instanceId, [param: MarshalAs(UnmanagedType.LPStr)] string name, IntPtr valueHandle);
		[DllImport(DynamicLibrary.Name, CharSet = CharSet.Ansi, EntryPoint = "conct_controller_call_function")]
		public static extern IntPtr CallFunction(IntPtr handle, IntPtr addressHandle, UInt16 instanceId, [param: MarshalAs(UnmanagedType.LPStr)] string name, IntPtr values, int valueCount);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_controller_release_command")]
		public static extern void ReleaseCommand(IntPtr handle, IntPtr commandHandle);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_controller_register_command_callback")]
		public static extern void RegisterCommandCallback(IntPtr handle, IntPtr callback);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_controller_unregister_command_callback")]
		public static extern void UnregisterCommandCallback(IntPtr handle, IntPtr callback);
	}
}