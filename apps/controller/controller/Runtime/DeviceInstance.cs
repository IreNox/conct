using System;
using System.Collections.Generic;
using System.Text;

namespace conct
{
    public class DeviceInstance
    {
		private ConnectedDevice m_device;
		private Instance m_instance;
		private InterfaceType m_type;

		public DeviceInstance(ConnectedDevice device, Instance instance)
		{
			m_device = device;
			m_instance = instance;

			m_type = App.System.Types.FindInterfaceByCrc(instance.TypeCrc);
			if (m_type == null)
			{
				throw new Exception("Type not found for crc: " + instance.TypeCrc.ToString());
			}
		}

		public ConnectedDevice Device
		{
			get { return m_device; }
		}

		public Instance Instance
		{
			get { return m_instance; }
		}

		public InterfaceType Type
		{
			get { return m_type; }
		}
    }
}
