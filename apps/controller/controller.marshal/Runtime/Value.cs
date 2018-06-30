using System;

namespace conct
{
	public class Value : IDisposable
	{
		private IntPtr m_nativeInstance;

		public Value()
		{
			m_nativeInstance = ValueNative.Create();
		}

		internal Value(IntPtr nativeInstance)
		{
			m_nativeInstance = nativeInstance;
		}

		public void Dispose()
		{
			if (m_nativeInstance != IntPtr.Zero)
			{
				ValueNative.Destroy(m_nativeInstance);
				m_nativeInstance = IntPtr.Zero;
			}
		}

		public IntPtr NativeInstance
		{
			get { return m_nativeInstance; }
		}

		public ValueType Type
		{
			get { return ValueNative.GetType(m_nativeInstance); }
		}

		public bool Boolean
		{
			get { return ValueNative.GetBoolean(m_nativeInstance); }
			set { ValueNative.SetBoolean(m_nativeInstance, value); }
		}

		public int Integer
		{
			get { return ValueNative.GetInteger(m_nativeInstance); }
			set { ValueNative.SetInteger(m_nativeInstance, value); }
		}

		public uint Unsigned
		{
			get { return ValueNative.GetUnsigned(m_nativeInstance); }
			set { ValueNative.SetUnsigned(m_nativeInstance, value); }
		}

		public string String
		{
			get { return ValueNative.GetString(m_nativeInstance); }
			//set { ValueNative.SetString(m_nativeInstance, value); }
		}

		public UInt16 PercentValue
		{
			get { return ValueNative.GetPercentValue(m_nativeInstance); }
			set { ValueNative.SetPercentValue(m_nativeInstance, value); }
		}

		public UInt32 Guid
		{
			get { return ValueNative.GetGuid(m_nativeInstance); }
			set { ValueNative.SetGuid(m_nativeInstance, value); }
		}

		public Byte DeviceId
		{
			get { return ValueNative.GetDeviecId(m_nativeInstance); }
			set { ValueNative.SetDeviecId(m_nativeInstance, value); }
		}

		public Instance Instance
		{
			get { return ValueNative.GetInstance(m_nativeInstance); }
			set { ValueNative.SetInstance(m_nativeInstance, value); }
		}

		public UInt16 TypeCrc
		{
			get { return ValueNative.GetTypeCrc(m_nativeInstance); }
			set { ValueNative.SetTypeCrc(m_nativeInstance, value); }
		}
	}
}