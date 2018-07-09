using System;
using System.Runtime.InteropServices;

namespace conct
{
	internal static class ArrayTypeNative
	{
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_array_type_get_base_type")]
		public static extern IntPtr GetBaseType(IntPtr handle);
	}
}