using System;
using System.Collections.Specialized;

using Xamarin.Forms;

namespace conct
{
    public sealed partial class DevicePage : ContentPage
    {
		private ConnectedDevice m_device;

        public DevicePage(ConnectedDevice device)
        {
			m_device = device;

            InitializeComponent();

			BindingContext = new DeviceViewModel(device);

			m_device.Instances.CollectionChanged += Instances_CollectionChanged;
			SyncInstances();
        }

		private void Instances_CollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
		{
			SyncInstances();
		}

		private void SyncInstances()
		{
			BaseLayout.Children.Sync(m_device.Instances, (s, t) => ((DeviceInstanceControl)t).Instance == s, s => new DeviceInstanceControl(s));
		}
    }
}
