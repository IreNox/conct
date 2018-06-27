using System;

namespace conct
{
	public class Type
	{
		protected IntPtr m_nativeInstance;

		protected Type(IntPtr nativeInstance)
		{
			m_nativeInstance = nativeInstance;
		}

		public string Name
		{
			get
			{
				return "";
				//return TypeCollectionNative.GetName(m_nativeInstance);
			}
		}
	}
}