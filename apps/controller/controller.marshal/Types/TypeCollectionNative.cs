using System;
using System.Runtime.InteropServices;

namespace conct
{
	internal static class TypeCollectionNative
	{
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_collection_create")]
		public static extern IntPtr Create();
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_collection_destroy")]
		public static extern void Destroy(IntPtr instance);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_collection_load")]
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool Load( IntPtr instance, [param: MarshalAs(UnmanagedType.LPStr)] string path );

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_collection_add_value_type")]
		public static extern IntPtr AddValueType(IntPtr instance, [param: MarshalAs(UnmanagedType.LPStr)] string namespaceVar, [param: MarshalAs(UnmanagedType.LPStr)] string name, [param: MarshalAs(UnmanagedType.LPStr)] string cppName, [param: MarshalAs(UnmanagedType.I4)] ValueType valueType );

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_collection_find_type")]
		public static extern IntPtr FindType(IntPtr instance, [param: MarshalAs(UnmanagedType.LPStr)] string fullname, [param: MarshalAs(UnmanagedType.LPStr)] string referenceNamespace);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_collection_find_type_by_crc")]
		public static extern IntPtr FindTypeByCrc(IntPtr instance, UInt16 typeCrc);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_collection_find_interface")]
		public static extern IntPtr FindInterface(IntPtr instance, [param: MarshalAs(UnmanagedType.LPStr)] string fullname, [param: MarshalAs(UnmanagedType.LPStr)] string referenceNamespace);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_collection_find_interface_by_crc")]
		public static extern IntPtr FindInterfaceByCrc(IntPtr instance, UInt16 typeCrc);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_collection_find_struct")]
		public static extern IntPtr FindStruct(IntPtr instance, [param: MarshalAs(UnmanagedType.LPStr)] string fullname, [param: MarshalAs(UnmanagedType.LPStr)] string referenceNamespace);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_collection_find_struct_by_crc")]
		public static extern IntPtr FindStructByCrc(IntPtr instance, UInt16 typeCrc);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_collection_make_array")]
		public static extern IntPtr MakeArray(IntPtr instance, IntPtr type);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_collection_get_type_count")]
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool GetTypeCount(IntPtr instance);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_collection_find_type_by_index")]
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool GetTypeByIndex(IntPtr instance, int index);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_collection_get_interface_count")]
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool GetInterfaceCount(IntPtr instance);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_collection_find_interface_by_index")]
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool GetInterfaceByIndex(IntPtr instance, int index);

		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_collection_get_struct_count")]
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool getStructCount(IntPtr instance);
		[DllImport(DynamicLibrary.Name, EntryPoint = "conct_type_collection_find_struct_by_index")]
		[return: MarshalAs(UnmanagedType.U1)]
		public static extern bool GetStructByIndex(IntPtr instance, int index);
	}
}