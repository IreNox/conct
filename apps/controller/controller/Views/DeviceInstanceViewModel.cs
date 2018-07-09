using System;
using System.Collections.Generic;
using System.Text;

namespace conct
{
    public class DeviceInstanceViewModel
    {
		private DeviceInstance m_instance;

		public DeviceInstanceViewModel(DeviceInstance instance)
		{
			m_instance = instance;
		}

		public string TypeName
		{
			get { return m_instance.Type.Name; }
		}
	}
}
