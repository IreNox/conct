using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Text;

namespace conct
{
    public class DeviceViewModel : INotifyPropertyChanged
    {
		public event PropertyChangedEventHandler PropertyChanged;

		private ConnectedDevice m_device;

		public DeviceViewModel(ConnectedDevice device)
		{
			m_device = device;
			m_device.PropertyChanged += Device_PropertyChanged;
		}

		public String Title
		{
			get { return m_device.Name; }
		}

		private void Device_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			if (PropertyChanged == null)
			{
				return;
			}

			if (e.PropertyName == "Name")
			{
				PropertyChanged(this, new PropertyChangedEventArgs("Title"));
			}
		}
	}
}
