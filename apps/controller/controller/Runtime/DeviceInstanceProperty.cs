using System;

namespace conct
{
	public class DeviceInstanceProperty
    {
		private enum CommandType
		{
			Get,
			Set
		}

		private DeviceInstance m_instance;
		private InterfaceProperty m_property;

		private CommandType m_commandType;
		private Command m_command;

		private Value m_value;
		private bool m_hasValueChanged;

		public DeviceInstanceProperty(DeviceInstance instance, InterfaceProperty property)
		{
			m_instance = instance;
			m_property = property;

			Controller controller = App.System.Controller;
			controller.CommandChanged += Controller_CommandChanged;

			m_commandType = CommandType.Get;
			m_command = App.System.Controller.GetProperty(instance.Device.Address, instance.Instance.InstanceId, m_property.Name);
		}

		public InterfaceProperty Property
		{
			get { return m_property; }
		}

		public Value Value
		{
			get { return m_value; }
			set
			{
				if (value.Type != m_property.Type.ValueType)
				{
					throw new Exception("Value type doesn't match.");
				}

				if (m_value != null && m_value != value)
				{
					m_value.Dispose();
				}

				m_value = value;
				startSetCommand();
			}
		}

		private void Controller_CommandChanged(Controller controller, Command command)
		{
			if(command != m_command)
			{
				return;
			}

			if (command.IsOk)
			{
				if (m_commandType == CommandType.Get)
				{
					if (m_value != null)
					{
						m_value.Dispose();
					}

					m_value = command.ResultValue;
				}

				m_instance.OnPropertyChanged(this);
			}
			else
			{
				// display error
			}

			command.Dispose();
			m_command = null;

			if (m_hasValueChanged)
			{
				startSetCommand();
			}
		}

		private void startSetCommand()
		{
			if(m_command != null)
			{
				m_hasValueChanged = true;
				return;
			}

			m_hasValueChanged = false;

			m_commandType = CommandType.Set;
			m_command = App.System.Controller.SetProperty(m_instance.Device.Address, m_instance.Instance.InstanceId, m_property.Name, m_value);
		}
	}
}
