using System;
using System.Runtime.InteropServices;

namespace conct
{
	public enum TypeDescription
	{
		Value,
		Interface,
		Struct,
		Enum,
		Array
	}

	internal static class TypeNative
	{
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_get_namespace", CallingConvention=CallingConvention.StdCall)]
		[return: MarshalAs(UnmanagedType.LPStr)]
		public static extern string GetNamespace(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_get_name")]
		[return: MarshalAs(UnmanagedType.LPStr)]
		public static extern string GetName(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_get_cpp_name")]
		[return: MarshalAs(UnmanagedType.LPStr)]
		public static extern string GetCppName(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_get_full_name")]
		[return: MarshalAs(UnmanagedType.LPStr)]
		public static extern string GetFullName(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_get_header_filename")]
		[return: MarshalAs(UnmanagedType.LPStr)]
		public static extern string GetHeaderFilename(IntPtr handle);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_get_description")]
		[return: MarshalAs(UnmanagedType.I4)]
		public static extern TypeDescription GetDescription(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_get_value_type")]
		[return: MarshalAs(UnmanagedType.I4)]
		public static extern ValueType GetValueType(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_get_crc")]
		public static extern UInt16 GetCrc(IntPtr handle);
	}
}