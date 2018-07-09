using System.Collections.Specialized;
using System.Collections.ObjectModel;

namespace conct
{
    public class ServerListViewModel
	{
		private ObservableCollection<ServerViewModel> m_servers;

		public ServerListViewModel()
		{
			m_servers = new ObservableCollection<ServerViewModel>();
			SyncServers();

			App.System.Servers.CollectionChanged += Servers_CollectionChanged;
		}

		public ObservableCollection<ServerViewModel> Servers
		{
			get { return m_servers; }
		}

		private void Servers_CollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
		{
			SyncServers();
		}

		private void SyncServers()
		{
			m_servers.Sync(App.System.Servers, (s, t) => t.Data == s, s => new ServerViewModel(s));
		}
	}
}
