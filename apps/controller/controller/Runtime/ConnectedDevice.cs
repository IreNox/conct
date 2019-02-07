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
		private Command m_devicesCommand;

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
			if (command != m_nameCommand &&
				command != m_instancesCommand &&
				command != m_devicesCommand)
			{
				return;
			}

			if (command.IsOk)
			{
				if (command == m_nameCommand)
				{
					m_name = command.ResultValue.String;
					OnPropertyChanged("Name");

					m_nameCommand = null;
				}
				else if (command == m_instancesCommand)
				{
					Type routerType = App.System.Types.FindType("Core.Router", "");

					Value value = command.ResultValue;
					object[] instances = value.GetArray(App.System.Types);

					foreach (Instance instance in instances.Cast<Instance>())
					{
						m_instances.Add(new DeviceInstance(this, instance));

						if (instance.TypeCrc == routerType.Crc)
						{
							m_devicesCommand = App.System.Controller.GetProperty(m_address, instance.InstanceId, "ConnectedDevices");
						}
					}

					m_instancesCommand = null;
				}
				else if (command == m_devicesCommand)
				{
					Value value = command.ResultValue;
					object[] instances = value.GetArray(App.System.Types);

					foreach (byte deviceId in instances.Cast<byte>())
					{
						DeviceAddress address = new DeviceAddress(m_address);
						address.Push(deviceId);

						if(App.System.Device.IsThisDevice(address))
						{
							continue;
						}

						App.System.Devices.Add(new ConnectedDevice(address));
					}

					m_devicesCommand = null;
				}
			}
			else
			{
				Console.WriteLine("Command failed");
				// display error
			}

			if (m_nameCommand == null && m_instancesCommand == null && m_devicesCommand == null)
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