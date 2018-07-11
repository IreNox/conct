using System;
using System.Runtime.InteropServices;

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
			set { ValueNative.SetString(m_nativeInstance, value); }
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

		public object GetStruct(TypeCollection types)
		{
			UInt16 structTypeCrc = ValueNative.GetStructType(m_nativeInstance);
			StructType structType = types.FindStructByCrc(structTypeCrc);
			if(structType == null || structType.ManagedType == null)
			{
				return null;
			}

			if (ValueNative.GetStructSize(m_nativeInstance) != Marshal.SizeOf(structType.ManagedType))
			{
				return null;
			}

			IntPtr structData = ValueNative.GetStructData(m_nativeInstance);
			return Marshal.PtrToStructure(structData, structType.ManagedType);
		}

		public void SetStruct(TypeCollection types, object data)
		{
			if (data == null)
			{
				throw new ArgumentNullException("data");
			}

			Type type = types.FindTypeByManagedType(data.GetType());
			if (type == null)
			{
				throw new Exception("Type not found for " + data.GetType().FullName);
			}

			int structSize = Marshal.SizeOf(type.ManagedType);
			IntPtr structData = Marshal.AllocHGlobal(structSize);

			Marshal.StructureToPtr(data, structData, false);
			ValueNative.SetStruct(m_nativeInstance, structData, structSize, type.Crc);

			Marshal.FreeHGlobal(structData);
		}

		public object[] GetArray(TypeCollection types)
		{
			UInt16 arrayTypeCrc = ValueNative.GetArrayType(m_nativeInstance);
			Type arrayType = types.FindTypeByCrc(arrayTypeCrc);
			if (arrayType == null || arrayType.ManagedType == null)
			{
				return null;
			}

			int elementSize = ValueNative.GetArrayElementSize(m_nativeInstance);
			if ( elementSize != Marshal.SizeOf(arrayType.ManagedType))
			{
				return null;
			}

			int length = ValueNative.GetArrayLength(m_nativeInstance);
			object[] array = new object[length];

			IntPtr arrayData = ValueNative.GetArrayData(m_nativeInstance);
			for (int i = 0; i < length; i++)
			{
				array[i] = Marshal.PtrToStructure(arrayData, arrayType.ManagedType);
				arrayData += elementSize;
			}

			return array;
		}

		public void SetArray(TypeCollection types, object[] array, System.Type managedType = null)
		{
			if (array == null)
			{
				throw new ArgumentNullException("array");
			}

			if (managedType == null && array.Length == 0)
			{
				throw new ArgumentNullException("managedType");
			}

			if (managedType == null)
			{
				managedType = array[0].GetType();
			}

			Type type = types.FindTypeByManagedType(managedType);
			if (type == null)
			{
				throw new Exception("Type not found for " + managedType.FullName);
			}

			int elementSize = Marshal.SizeOf(managedType);
			if (array.Length == 0)
			{
				ValueNative.SetArray(m_nativeInstance, IntPtr.Zero, elementSize, 0, type.Crc);
				return;
			}

			IntPtr arrayData = Marshal.AllocHGlobal(elementSize * array.Length);
			for (int i = 0; i < array.Length; i++)
			{
				if(array[i] == null)
				{
					throw new ArgumentNullException("array[" + i.ToString() + "]");
				}

				if (array[i].GetType() != managedType)
				{
					throw new Exception("Type '" + array[i].GetType().FullName + "' in array[" + i.ToString() + "] doesn't match given Type '" + managedType.FullName + "'");
				}

				Marshal.StructureToPtr(array[i], arrayData, false);
				arrayData += elementSize;
			}

			ValueNative.SetArray(m_nativeInstance, arrayData, elementSize, array.Length, type.Crc);
			Marshal.FreeHGlobal(arrayData);
		}
	}
}