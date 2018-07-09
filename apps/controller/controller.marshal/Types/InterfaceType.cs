using System;

namespace conct
{
	public struct InterfaceProperty
	{
		public string Name;
		public Type Type;
		public bool HasGetter;
		public bool HasSetter;
	}

	public struct InterfaceFunction
	{
		public string Name;
		public Type ReturnType;
		public InterfaceFunctionParameter[] Parameters;
	}

	public struct InterfaceFunctionParameter
	{
		public string Name;
		public Type Type;
	}

	public class InterfaceType : Type
    {
		private TypeCollection m_collection;

		private InterfaceProperty[] m_properties;
		private InterfaceFunction[] m_functions;

		public InterfaceType(IntPtr nativeInstance, TypeCollection collection)
			: base(nativeInstance)
		{
			m_collection = collection;
        }

		public Type BaseType
		{
			get { return m_collection.GetTypeFromHandle(InterfaceTypeNative.GetBaseType(m_nativeInstance)); }
		}

		public bool Internal
		{
			get { return InterfaceTypeNative.GetInternal(m_nativeInstance); }
		}

		public bool Singleton
		{
			get { return InterfaceTypeNative.GetSingleton(m_nativeInstance); }
		}

		public InterfaceProperty[] Properties
		{
			get
			{
				BuildProperties();
				return m_properties;
			}
		}

		public InterfaceFunction[] Functions
		{
			get
			{
				BuildFunctions();
				return m_functions;
			}
		}

		private void BuildProperties()
		{
			if (m_properties != null)
			{
				return;
			}

			int count = InterfaceTypeNative.GetPropertyCount(m_nativeInstance);
			m_properties = new InterfaceProperty[count];
			for (int i = 0; i < count; i++)
			{
				InterfaceProperty prop;
				prop.Name = InterfaceTypeNative.GetPropertyName(m_nativeInstance, i);
				prop.Type = m_collection.GetTypeFromHandle(InterfaceTypeNative.GetPropertyType(m_nativeInstance, i));
				prop.HasGetter = InterfaceTypeNative.GetPropertyHasGetter(m_nativeInstance, i);
				prop.HasSetter = InterfaceTypeNative.GetPropertyHasSetter(m_nativeInstance, i);

				m_properties[i] = prop;
			}
		}

		private void BuildFunctions()
		{
			if (m_functions != null)
			{
				return;
			}

			int count = InterfaceTypeNative.GetFunctionCount(m_nativeInstance);
			m_functions = new InterfaceFunction[count];
			for (int functionIndex = 0; functionIndex < count; functionIndex++)
			{
				InterfaceFunction func;
				func.Name = InterfaceTypeNative.GetFunctionName(m_nativeInstance, functionIndex);
				func.ReturnType = m_collection.GetTypeFromHandle(InterfaceTypeNative.GetFunctionReturnType(m_nativeInstance, functionIndex));

				int parameterCount = InterfaceTypeNative.GetFunctionParameterCount(m_nativeInstance, functionIndex);
				func.Parameters = new InterfaceFunctionParameter[parameterCount];
				for(int parameterIndex = 0; parameterIndex < parameterCount; parameterIndex++)
				{
					InterfaceFunctionParameter param;
					param.Name = InterfaceTypeNative.GetFunctionParameterName(m_nativeInstance, functionIndex, parameterIndex);
					param.Type = m_collection.GetTypeFromHandle(InterfaceTypeNative.GetFunctionParameterType(m_nativeInstance, functionIndex, parameterIndex));

					func.Parameters[parameterIndex] = param;
				}

				m_functions[functionIndex] = func;
			}
		}
	}
}