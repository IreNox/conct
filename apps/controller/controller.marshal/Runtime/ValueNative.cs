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
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool GetBoolean(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_integer")]
		public static extern int GetInteger(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_unsigned")]
		public static extern uint GetUnsigned(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_string")]
		public static extern IntPtr GetString(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_percent_value")]
		public static extern UInt16 GetPercentValue(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_device_id")]
		public static extern Byte GetDeviecId(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_type_crc")]
		public static extern UInt16 GetTypeCrc(IntPtr handle);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_struct_data")]
		public static extern IntPtr GetStructData(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_struct_size")]
		public static extern int GetStructSize(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_struct_type")]
		public static extern UInt16 GetStructType(IntPtr handle);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_array_data")]
		public static extern IntPtr GetArrayData(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_array_element_size")]
		public static extern int GetArrayElementSize(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_array_length")]
		public static extern int GetArrayLength(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_get_array_type")]
		public static extern UInt16 GetArrayType(IntPtr handle);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_void")]
		public static extern void SetVoid(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_boolean")]
		public static extern void SetBoolean(IntPtr handle, bool value);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_integer")]
		public static extern void SetInteger(IntPtr handle, int value);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_unsigned")]
		public static extern void SetUnsigned(IntPtr handle, uint value);
		[DllImport(DynamicLibrary.Name, CharSet = CharSet.Ansi, EntryPoint = "conct_value_set_string")]
		public static extern void SetString(IntPtr handle, [param: MarshalAs(UnmanagedType.LPStr)] string value);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_percent_value")]
		public static extern void SetPercentValue(IntPtr handle, UInt16 value);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_device_id")]
		public static extern void SetDeviecId(IntPtr handle, Byte value);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_type_crc")]
		public static extern void SetTypeCrc(IntPtr handle, UInt16 value);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_struct")]
		public static extern void SetStruct(IntPtr handle, IntPtr data, int size, UInt16 type);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_value_set_array")]
		public static extern void SetArray(IntPtr handle, IntPtr data, int elementSize, int length, UInt16 type);
	}
}