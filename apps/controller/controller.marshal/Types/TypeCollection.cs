using System;
using System.Collections.Generic;
using System.Linq;

namespace conct
{
	public class TypeCollection : IDisposable
	{
		private List<Type> m_types;
		private List<InterfaceType> m_interfaces;
		private List<StructType> m_structs;
		private List<ArrayType> m_arrays;

		private IntPtr m_nativeInstance;

		public TypeCollection()
		{
			m_types = new List<Type>();
			m_interfaces = new List<InterfaceType>();
			m_structs = new List<StructType>();
			m_arrays = new List<ArrayType>();

			m_nativeInstance = TypeCollectionNative.Create();
		}

		public void Dispose()
		{
			if (m_nativeInstance != IntPtr.Zero)
			{
				TypeCollectionNative.Destroy(m_nativeInstance);
				m_nativeInstance = IntPtr.Zero;
			}
		}

		public bool Load(string path)
		{
			return TypeCollectionNative.Load(m_nativeInstance, path);
		}

		public Type FindType(string name, string referenceNamespace)
		{
			IntPtr typeHandle = TypeCollectionNative.FindType(m_nativeInstance, name, referenceNamespace);
			return GetTypeFromHandle(typeHandle);
		}

		public Type FindTypeByCrc(UInt16 typeCrc)
		{
			IntPtr typeHandle = TypeCollectionNative.FindTypeByCrc(m_nativeInstance, typeCrc);
			return GetTypeFromHandle(typeHandle);
		}

		public InterfaceType FindInterface(string name, string referenceNamespace)
		{
			IntPtr typeHandle = TypeCollectionNative.FindInterface(m_nativeInstance, name, referenceNamespace);
			return GetTypeFromHandle(typeHandle) as InterfaceType;
		}

		public InterfaceType FindInterfaceByCrc(UInt16 typeCrc)
		{
			IntPtr typeHandle = TypeCollectionNative.FindInterfaceByCrc(m_nativeInstance, typeCrc);
			return GetTypeFromHandle(typeHandle) as InterfaceType;
		}

		public StructType FindStruct(string name, string referenceNamespace)
		{
			IntPtr typeHandle = TypeCollectionNative.FindStruct(m_nativeInstance, name, referenceNamespace);
			return GetTypeFromHandle(typeHandle) as StructType;
		}

		public ArrayType MakeArray(Type baseType)
		{
			IntPtr typeHandle = TypeCollectionNative.MakeArray(m_nativeInstance, baseType.NativeInstance);
			return GetTypeFromHandle(typeHandle) as ArrayType;
		}

		public Type GetTypeFromHandle(IntPtr typeHandle)
		{
			if(typeHandle == IntPtr.Zero)
			{
				return null;
			}

			Type type = m_types.FirstOrDefault(t => t.NativeInstance == typeHandle);
			if( type != null)
			{
				return type;
			}

			TypeDescription description = TypeNative.GetDescription(typeHandle);
			switch (description)
			{
				case TypeDescription.Value:
					type = new Type(typeHandle);
					break;

				case TypeDescription.Interface:
					InterfaceType interfaceType = new InterfaceType(typeHandle, this);
					m_interfaces.Add(interfaceType);
					type = interfaceType;
					break;

				case TypeDescription.Struct:
					StructType structType = new StructType(typeHandle, this);
					m_structs.Add(structType);
					type = structType;
					break;

				case TypeDescription.Enum:
					throw new NotImplementedException();

				case TypeDescription.Array:
					ArrayType arrayType = new ArrayType(typeHandle, this);
					m_arrays.Add(arrayType);
					type = arrayType;
					break;
			}

			m_types.Add(type);
			return type;
		}
	}
}