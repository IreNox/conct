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
    public partial class ServerListPage : ContentPage
    {
        public ServerListPage()
        {
            InitializeComponent();
			BindingContext = new ServerListViewModel();

			ToolbarItems.Add(new ToolbarItem("Add Server", null, AddServer));
		}

		private async void Server_Selected(object sender, SelectedItemChangedEventArgs e)
		{
			if (e.SelectedItem == null)
			{
				return;
			}

			ServerData server = ((ServerViewModel)e.SelectedItem).Data;

			List<string> options = new List<string>();
			options.Add("Edit");

			if(server.Handle == IntPtr.Zero)
			{
				options.Add("Reconnect");
			}

			options.Add("Delete");

			var action = await DisplayActionSheet("Select a action", "Cancel", null, options.ToArray());
			if (action == "Edit")
			{
				await Navigation.PushAsync(new ServerPage(server));
			}
			else if (action == "Reconnect")
			{
				App.System.ReconnectServer(server);
			}
			else if (action == "Delete")
			{
				App.System.RemoveServer(server);
			}

			ListView listView = (ListView)sender;
			listView.SelectedItem = null;
		}

		private void AddServer()
		{
			Navigation.PushAsync(new ServerPage(null));
		}
	}
}