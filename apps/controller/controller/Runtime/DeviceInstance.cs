using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;

namespace conct
{
    public class DeviceInstance
    {
		public event Event<DeviceInstance, DeviceInstanceProperty> PropertyChanged;

		private ConnectedDevice m_device;
		private Instance m_instance;
		private InterfaceType m_type;
		private List<DeviceInstanceProperty> m_properties;

		public DeviceInstance(ConnectedDevice device, Instance instance)
		{
			m_device = device;
			m_instance = instance;

			m_type = App.System.Types.FindInterfaceByCrc(instance.TypeCrc);
			if (m_type == null)
			{
				throw new Exception("Type not found for crc: " + instance.TypeCrc.ToString());
			}

			m_properties = m_type.Properties.Select(p => new DeviceInstanceProperty(this, p)).ToList();
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

		public IEnumerable<DeviceInstanceProperty> Properties
		{
			get { return m_properties; }
		}

		public void OnPropertyChanged(DeviceInstanceProperty property)
		{
			if (PropertyChanged != null)
			{
				PropertyChanged(this, property);
			}
		}
	}
}
