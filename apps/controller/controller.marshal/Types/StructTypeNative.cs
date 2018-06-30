using System;
using System.Runtime.InteropServices;

namespace conct
{
	internal static class StructTypeNative
	{
		[DllImport("libconct", EntryPoint = "conct_struct_type_get_field_count")]
		public static extern int GetFieldCount(IntPtr handle);
		[DllImport("libconct", EntryPoint = "conct_struct_type_get_field_name")]
		[return: MarshalAs(UnmanagedType.LPUTF8Str)]
		public static extern string GetFieldName(IntPtr handle, int index);
		[DllImport("libconct", EntryPoint = "conct_struct_type_get_field_type")]
		public static extern IntPtr GetFieldType(IntPtr handle, int index);
	}
}