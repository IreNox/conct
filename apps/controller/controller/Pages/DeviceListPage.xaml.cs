using System;

using Xamarin.Forms;

namespace conct
{
	public partial class DeviceListPage : ContentPage
	{
		public DeviceListPage()
		{
			InitializeComponent();

			BindingContext = new DeviceListViewModel();

			ToolbarItems.Add(new ToolbarItem("Refresh", null, Refresh));
		}

		private void Device_Selected(object sender, SelectedItemChangedEventArgs e)
		{
			if (e.SelectedItem == null)
			{
				return;
			}

			ConnectedDevice device = (ConnectedDevice)e.SelectedItem;

			Navigation.PushAsync(new DevicePage(device));
			((ListView)sender).SelectedItem = null;
		}

		private void Refresh()
		{
			App.System.SyncDevices();
		}
	}
}