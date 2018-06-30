using System;
using System.Runtime.InteropServices;

namespace conct
{
	internal static class CommandNative
	{
		[DllImport("libconct", EntryPoint = "conct_command_is_busy")]
		public static extern bool IsBusy(IntPtr handle);
		[DllImport("libconct", EntryPoint = "conct_command_is_finish")]
		public static extern bool IsFinish(IntPtr handle);
		[DllImport("libconct", EntryPoint = "conct_command_is_ok")]
		public static extern bool IsOk(IntPtr handle);
		[DllImport("libconct", EntryPoint = "conct_command_is_failure")]
		public static extern bool IsFailure(IntPtr handle);

		[DllImport("libconct", EntryPoint = "conct_value_command_get_value")]
		public static extern IntPtr GetValue(IntPtr handle);
	}
}