using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System;

namespace conct
{
	public class ConnectedDevice : INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;

		private DeviceAddress m_address;

		private string m_name;
		private ObservableCollection<DeviceInstance> m_instances;

		private Command m_nameCommand;
		private Command m_instancesCommand;

		public ConnectedDevice(DeviceAddress address)
		{
			m_address = address;
			m_name = address.Text;
			m_instances = new ObservableCollection<DeviceInstance>();

			Controller controller = App.System.Controller;
			controller.CommandChanged += Controller_CommandChanged;

			m_nameCommand = controller.GetProperty(m_address, 0, "Name");
			m_instancesCommand = controller.GetProperty(m_address, 0, "Instances");
		}

		public string Name
		{
			get { return m_name; }
		}

		public DeviceAddress Address
		{
			get { return m_address; }
		}

		public string AddressText
		{
			get { return m_address.Text; }
		}

		public ObservableCollection<DeviceInstance> Instances
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

					m_nameCommand = null;
				}
				else if (command == m_instancesCommand)
				{
					Value value = m_instancesCommand.ResultValue;

					object[] instances = value.GetArray(App.System.Types);

					foreach (Instance instance in instances.Cast<Instance>())
					{
						m_instances.Add(new DeviceInstance(this, instance));
					}

					m_instancesCommand = null;
				}
			}
			else
			{
				// display error
			}

			if (m_nameCommand == null && m_instancesCommand == null)
			{
				sender.CommandChanged -= Controller_CommandChanged;
			}
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