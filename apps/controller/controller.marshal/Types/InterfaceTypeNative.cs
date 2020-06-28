using System;
using System.Runtime.InteropServices;

namespace conct
{
	internal static class InterfaceTypeNative
	{
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_interface_type_get_base_type")]
		public static extern IntPtr GetBaseType(IntPtr handle);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_interface_type_get_internal")]
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool GetInternal(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_interface_type_get_singleton")]
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool GetSingleton(IntPtr handle);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_interface_type_get_property_count")]
		public static extern int GetPropertyCount(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_interface_type_get_property_name")]
		public static extern IntPtr GetPropertyName(IntPtr handle, int index);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_interface_type_get_property_type")]
		public static extern IntPtr GetPropertyType(IntPtr handle, int index);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_interface_type_get_property_has_getter")]
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool GetPropertyHasGetter(IntPtr handle, int index);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_interface_type_get_property_has_setter")]
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool GetPropertyHasSetter(IntPtr handle, int index);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_interface_type_get_function_count")]
		public static extern int GetFunctionCount(IntPtr handle);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_interface_type_get_function_name")]
		public static extern IntPtr GetFunctionName(IntPtr handle, int functionIndex);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_interface_type_get_function_return_type")]
		public static extern IntPtr GetFunctionReturnType(IntPtr handle, int functionIndex);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_interface_type_get_function_parameter_count")]
		public static extern int GetFunctionParameterCount(IntPtr handle, int functionIndex);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_interface_type_get_function_parameter_name")]
		public static extern IntPtr GetFunctionParameterName(IntPtr handle, int functionIndex, int parameterIndex);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_interface_type_get_function_parameter_type")]
		public static extern IntPtr GetFunctionParameterType(IntPtr handle, int functionIndex, int parameterIndex);
	}
}