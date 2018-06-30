using System;

namespace conct
{
	public struct StructField
	{
		public string Name;
		public Type Type;
	}

	public class StructType : Type
	{
		private TypeCollection m_collection;

		private StructField[] m_fields;

		public StructType(IntPtr nativeInstance, TypeCollection collection)
			: base(nativeInstance)
		{
			m_collection = collection;
		}

		public StructField[] Fields
		{
			get
			{
				BuildFields();
				return m_fields;
			}
		}

		private void BuildFields()
		{
			if (m_fields != null)
			{
				return;
			}

			int count = StructTypeNative.GetFieldCount(m_nativeInstance);
			m_fields = new StructField[count];
			for (int i = 0; i < count; i++)
			{
				StructField field;
				field.Name = StructTypeNative.GetFieldName(m_nativeInstance, i);
				field.Type = m_collection.GetTypeFromHandle(StructTypeNative.GetFieldType(m_nativeInstance, i));

				m_fields[i] = field;
			}
		}
	}
}