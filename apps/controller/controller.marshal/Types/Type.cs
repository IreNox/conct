using System;
using System.Runtime.InteropServices;

namespace conct
{
	public class Type
	{
		protected IntPtr m_nativeInstance;
		private System.Type m_managedType;

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
			get { return Marshal.PtrToStringAnsi(TypeNative.GetNamespace(m_nativeInstance)); }
		}

		public string Name
		{
			get { return Marshal.PtrToStringAnsi(TypeNative.GetName(m_nativeInstance)); }
		}

		public string CppName
		{
			get { return Marshal.PtrToStringAnsi(TypeNative.GetCppName(m_nativeInstance)); }
		}

		public string FullName
		{
			get { return Marshal.PtrToStringAnsi(TypeNative.GetFullName(m_nativeInstance)); }
		}

		public string HeaderFilename
		{
			get { return Marshal.PtrToStringAnsi(TypeNative.GetHeaderFilename(m_nativeInstance)); }
		}

		public TypeKind Kind
		{
			get { return TypeNative.GetKind(m_nativeInstance); }
		}

		public ValueType ValueType
		{
			get { return TypeNative.GetValueType(m_nativeInstance); }
		}

		public UInt16 Crc
		{
			get { return TypeNative.GetCrc(m_nativeInstance); }
		}

		public System.Type ManagedType
		{
			get { return m_managedType; }
			set { m_managedType = value; }
		}
	}
}