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
	public partial class SettingsConnectionPage : ContentPage
	{
		private ServerData m_data;

		public SettingsConnectionPage(ServerData data)
		{
			InitializeComponent();

			m_data = data;

			if (m_data != null)
			{
				titleEntry.Text = data.Config.Title;
				hostnameEntry.Text = data.Config.Hostname;
				hostportEntry.Text = data.Config.HostPort.ToString();
			}
			else
			{
				hostportEntry.Text = "5489";
			}

			ToolbarItems.Add(new ToolbarItem("Add", null, Confirm));
		}

		private void Confirm()
		{
			if (m_data != null)
			{
				App.System.RemoveServer(m_data);
			}

			ServerConfig config = new ServerConfig();
			config.Title = titleEntry.Text;
			config.Hostname = hostnameEntry.Text;
			config.HostPort = UInt16.Parse(hostportEntry.Text);

			m_data = App.System.AddServer(config);
			Navigation.PopAsync();
		}
	}
}