using System;

namespace conct
{
	public class Type
	{
		protected IntPtr m_nativeInstance;

		public Type(IntPtr nativeInstance)
		{
			m_nativeInstance = nativeInstance;
		}

		public IntPtr NativeInstance
		{
			get { return m_nativeInstance; }
		}

		public string Namespace
		{
			get { return TypeNative.GetNamespace(m_nativeInstance); }
		}

		public string Name
		{
			get { return TypeNative.GetName(m_nativeInstance); }
		}

		public string CppName
		{
			get { return TypeNative.GetCppName(m_nativeInstance); }
		}

		public string FullName
		{
			get { return TypeNative.GetFullName(m_nativeInstance); }
		}

		public string HeaderFilename
		{
			get { return TypeNative.GetHeaderFilename(m_nativeInstance); }
		}

		public TypeDescription Description
		{
			get { return TypeNative.GetDescription(m_nativeInstance); }
		}

		public ValueType ValueType
		{
			get { return TypeNative.GetValueType(m_nativeInstance); }
		}

		public UInt16 Crc
		{
			get { return TypeNative.GetCrc(m_nativeInstance); }
		}
	}
}