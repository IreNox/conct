using System;
using System.Runtime.InteropServices;

namespace conct
{
	internal static class InterfaceTypeNative
	{
		[DllImport("libconct", EntryPoint = "conct_interface_type_get_base_type")]
		public static extern IntPtr GetBaseType(IntPtr handle);

		[DllImport("libconct", EntryPoint = "conct_interface_type_get_internal")]
		public static extern bool GetInternal(IntPtr handle);
		[DllImport("libconct", EntryPoint = "conct_interface_type_get_singleton")]
		public static extern bool GetSingleton(IntPtr handle);

		[DllImport("libconct", EntryPoint = "conct_interface_type_get_property_count")]
		public static extern int GetPropertyCount(IntPtr handle);
		[DllImport("libconct", EntryPoint = "conct_interface_type_get_property_name")]
		[return: MarshalAs(UnmanagedType.LPUTF8Str)]
		public static extern string GetPropertyName(IntPtr handle, int index);
		[DllImport("libconct", EntryPoint = "conct_interface_type_get_property_type")]
		public static extern IntPtr GetPropertyType(IntPtr handle, int index);
		[DllImport("libconct", EntryPoint = "conct_interface_type_get_property_has_getter")]
		public static extern bool GetPropertyHasGetter(IntPtr handle, int index);
		[DllImport("libconct", EntryPoint = "conct_interface_type_get_property_has_setter")]
		public static extern bool GetPropertyHasSetter(IntPtr handle, int index);

		[DllImport("libconct", EntryPoint = "conct_interface_type_get_function_count")]
		public static extern int GetFunctionCount(IntPtr handle);
		[DllImport("libconct", EntryPoint = "conct_interface_type_get_function_name")]
		[return: MarshalAs(UnmanagedType.LPUTF8Str)]
		public static extern string GetFunctionName(IntPtr handle, int functionIndex);
		[DllImport("libconct", EntryPoint = "conct_interface_type_get_function_return_type")]
		public static extern IntPtr GetFunctionReturnType(IntPtr handle, int functionIndex);
		[DllImport("libconct", EntryPoint = "conct_interface_type_get_function_parameter_count")]
		public static extern int GetFunctionParameterCount(IntPtr handle, int functionIndex);

		[DllImport("libconct", EntryPoint = "conct_interface_type_get_function_parameter_name")]
		[return: MarshalAs(UnmanagedType.LPUTF8Str)]
		public static extern string GetFunctionParameterName(IntPtr handle, int functionIndex, int parameterIndex);
		[DllImport("libconct", EntryPoint = "conct_interface_type_get_function_parameter_type")]
		public static extern IntPtr GetFunctionParameterType(IntPtr handle, int functionIndex, int parameterIndex);
	}
}