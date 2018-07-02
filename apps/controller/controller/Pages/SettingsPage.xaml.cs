using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace conct
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class SettingsPage : ContentPage
    {
        public SettingsPage()
        {
            InitializeComponent();
			BindingContext = new SettingsViewModel();

			ToolbarItems.Add(new ToolbarItem("Add Server", null, AddServer));
		}

		protected override void OnAppearing()
		{
			RefreshServers();
			base.OnAppearing();
		}

		private async void Server_Selected(object sender, SelectedItemChangedEventArgs e)
		{
			if (e.SelectedItem == null)
			{
				return;
			}

			ServerData server = ((ServerViewModel)e.SelectedItem).Data;

			var action = await DisplayActionSheet("Select a action", "Cancel", "Delete", "Edit");
			if(action == "Delete")
			{
				App.System.RemoveServer(server);
				RefreshServers();
			}
			else if(action == "Edit")
			{
				await Navigation.PushAsync(new SettingsConnectionPage(server));
			}

			ListView listView = (ListView)sender;
			listView.SelectedItem = null;
		}

		private void AddServer()
		{
			Navigation.PushAsync(new SettingsConnectionPage(null));
		}

		private void RefreshServers()
		{
			((SettingsViewModel)BindingContext).OnPropertyChanged("Servers");
		}
	}
}