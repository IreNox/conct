using System;
using System.Collections.Generic;

namespace conct
{
	public class TypeCollection
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

		~TypeCollection()
		{
			if (m_nativeInstance != IntPtr.Zero)
			{
				TypeCollectionNative.Destroy(m_nativeInstance);
			}
		}

		bool Load(string path)
		{
			return TypeCollectionNative.Load(m_nativeInstance, path);
		}
	}
}