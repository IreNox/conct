using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace conct
{
	public partial class DeviceListPage : ContentPage
	{
		public DeviceListPage()
		{
			InitializeComponent();

			this.BindingContext = new DeviceListViewModel();
		}

		private void Device_Selected(object sender, SelectedItemChangedEventArgs e)
		{
			if (e.SelectedItem == null)
			{
				return;
			}

			ConnectedDevice device = (ConnectedDevice)e.SelectedItem;

			Navigation.PushAsync(new DevicePage(device));
		}
	}
}