using System;
using System.Runtime.InteropServices;

namespace conct
{
	internal static class CommandNative
	{
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_command_is_busy")]
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool IsBusy(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_command_is_finish")]
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool IsFinish(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_command_is_ok")]
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool IsOk(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_command_is_failure")]
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool IsFailure(IntPtr handle);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_command_get_value")]
		public static extern IntPtr GetValue(IntPtr handle);
	}
}