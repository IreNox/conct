using System;
using System.Runtime.InteropServices;

namespace conct
{
	internal static class StructTypeNative
	{
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_struct_type_get_field_count")]
		public static extern int GetFieldCount(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_struct_type_get_field_name")]
		[return: MarshalAs(UnmanagedType.LPStr)]
		public static extern string GetFieldName(IntPtr handle, int index);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_struct_type_get_field_type")]
		public static extern IntPtr GetFieldType(IntPtr handle, int index);
	}
}