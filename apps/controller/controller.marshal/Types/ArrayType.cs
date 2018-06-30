using System;

namespace conct
{
	public class ArrayType : Type
	{
		private TypeCollection m_collection;

		public ArrayType(IntPtr nativeInstance, TypeCollection collection)
			: base(nativeInstance)
		{
			m_collection = collection;
		}

		Type BaseType
		{
			get { return m_collection.GetTypeFromHandle(ArrayTypeNative.GetBaseType(m_nativeInstance)); }
		}
	}
}