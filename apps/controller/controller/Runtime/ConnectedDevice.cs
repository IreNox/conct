using System;
using System.Collections.ObjectModel;
using System.ComponentModel;

namespace conct
{
	public class ConnectedDevice : INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;

		private DeviceAddress m_address;

		private string m_name;
		private ObservableCollection<Instance> m_instances;

		private Command m_nameCommand;
		private Command m_instancesCommand;

		public ConnectedDevice(DeviceAddress address)
		{
			m_address = address;
			m_name = address.Text;
			m_instances = new ObservableCollection<Instance>();

			Controller controller = App.System.Controller;
			controller.CommandChanged += Controller_CommandChanged;

			m_nameCommand = controller.GetProperty(m_address, 0, "Name");
			m_instancesCommand = controller.GetProperty(m_address, 0, "Instances");
		}

		public string Name
		{
			get { return m_name; }
		}

		public string Address
		{
			get { return m_address.Text; }
		}

		public ObservableCollection<Instance> Instances
		{
			get { return m_instances; }
		}

		private void Controller_CommandChanged(Controller sender, Command command)
		{
			if (command != m_nameCommand && command != m_instancesCommand)
			{
				return;
			}

			if (command.IsOk)
			{
				if (command == m_nameCommand)
				{
					m_name = m_nameCommand.ResultValue.String;
					OnPropertyChanged("Name");
				}
				else if (command == m_instancesCommand)
				{
					//m_instancesCommand.ResultValue.Instance
				}
			}
			else
			{
				// display error
			}

			sender.CommandChanged -= Controller_CommandChanged;
			command.Dispose();
		}

		private void OnPropertyChanged(string propertyName)
		{
			if(PropertyChanged != null)
			{
				PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
			}
		}
	}
}