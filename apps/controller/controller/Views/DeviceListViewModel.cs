using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;

namespace conct
{
    public class DeviceListViewModel
	{
		public DeviceListViewModel()
		{
		}

        public ObservableCollection<ConnectedDevice> Devices
        {
            get { return App.System.Devices; }
        }
    }
}
