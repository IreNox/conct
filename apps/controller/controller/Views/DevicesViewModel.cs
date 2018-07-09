using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;

namespace conct
{
    public class DevicesViewModel
	{
		public DevicesViewModel()
		{
		}

        public ObservableCollection<ConnectedDevice> Devices
        {
            get { return App.System.Devices; }
        }
    }
}
