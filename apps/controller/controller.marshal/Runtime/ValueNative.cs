using System;
using System.Runtime.InteropServices;

namespace conct
{
	internal static class ValueNative
	{
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_create")]
		public static extern IntPtr Create();
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_destroy")]
		public static extern void Destroy(IntPtr handle);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_type")]
		[return: MarshalAs(UnmanagedType.I4)]
		public static extern ValueType GetType(IntPtr handle);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_boolean")]
		public static extern bool GetBoolean(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_integer")]
		public static extern int GetInteger(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_unsigned")]
		public static extern uint GetUnsigned(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_string")]
		[return: MarshalAs(UnmanagedType.LPStr)]
		public static extern string GetString(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_percent_value")]
		public static extern UInt16 GetPercentValue(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_guid")]
		public static extern UInt32 GetGuid(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_device_id")]
		public static extern Byte GetDeviecId(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_instance")]
		[return: MarshalAs(UnmanagedType.Struct)]
		public static extern Instance GetInstance(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_type_crc")]
		public static extern UInt16 GetTypeCrc(IntPtr handle);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_void")]
		public static extern void SetVoid(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_boolean")]
		public static extern void SetBoolean(IntPtr handle, bool value);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_integer")]
		public static extern void SetInteger(IntPtr handle, int value);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_unsigned")]
		public static extern void SetUnsigned(IntPtr handle, uint value);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_string")]
		public static extern void SetString(IntPtr handle, [param: MarshalAs(UnmanagedType.LPStr)] string value);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_percent_value")]
		public static extern void SetPercentValue(IntPtr handle, UInt16 value);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_guid")]
		public static extern void SetGuid(IntPtr handle, UInt32 value);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_device_id")]
		public static extern void SetDeviecId(IntPtr handle, Byte value);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_instance")]
		public static extern void SetInstance(IntPtr handle, [param: MarshalAs(UnmanagedType.Struct)] Instance value);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_type_crc")]
		public static extern void SetTypeCrc(IntPtr handle, UInt16 value);
	}
}