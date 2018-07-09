using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml;
using System.Xml.Serialization;
using System.Collections.ObjectModel;

using Android.Util;

namespace conct
{
	public class Conct : IDisposable
	{
		private Device m_device;
		private Controller m_controller;

		private TypeCollection m_typeCollection;

		private string m_configPath;
		private ObservableCollection<ServerData> m_servers;
		private ObservableCollection<ConnectedDevice> m_devices;

		public Conct()
		{
			m_device = new Device();
			m_controller = m_device.Controller;
			m_typeCollection = new TypeCollection();
			m_devices = new ObservableCollection<ConnectedDevice>();
		}

		public void Dispose()
		{
			if (m_controller != null)
			{
				m_controller.Dispose();
				m_controller = null;
			}

			if (m_device != null)
			{
				m_device.Dispose();
				m_device = null;
			}

			if (m_typeCollection != null)
			{
				m_typeCollection.Dispose();
				m_typeCollection = null;
			}
		}

		public bool Load(string configPath, string typesPath)
		{
			m_configPath = configPath;

			bool result = true;
			result &= LoadConfig();
			result &= m_typeCollection.Load(typesPath);

			SyncDevices();

			if (!result)
			{
				App.Current.MainPage.DisplayAlert("conct.controller", "Failed to load configuration!", "Close");
			}

			return result;
		}

		public void Loop()
		{
			m_device.Loop();
		}

		public TypeCollection Types
		{
			get { return m_typeCollection; }
		}

		public Controller Controller
		{
			get { return m_controller; }
		}

		public ObservableCollection<ServerData> Servers
		{
			get { return m_servers; }
		}

		public ObservableCollection<ConnectedDevice> Devices
		{
			get { return m_devices; }
		}

		public ServerData AddServer(ServerConfig config)
		{
			ServerData data = AddServerInternal(config);
			SaveConfig();

			if (data.Handle != IntPtr.Zero)
			{
				SyncDevices();
			}

			return data;
		}

		private ServerData AddServerInternal(ServerConfig config)
		{
			ServerData data = new ServerData()
			{
				Config = config,
				Handle = IntPtr.Zero
			};

			ReconnectServer(data);

			m_servers.Add(data);
			return data;
		}

		public void ReconnectServer(ServerData data)
		{
			if (data.Handle != IntPtr.Zero)
			{
				return;
			}

			data.Handle = m_device.AddPort(data.Config.Hostname, data.Config.HostPort);

			if(data.Handle == IntPtr.Zero)
			{
				App.Current.MainPage.DisplayAlert("conct.controller", "Failed to connect to '" + data.Config.Hostname + ":" + data.Config.HostPort.ToString() + "'!", "Close");
			}
		}

		public void RemoveServer(ServerData data)
		{
			if (data.Handle != IntPtr.Zero)
			{
				m_device.RemovePort(data.Handle);
				SyncDevices();
			}

			m_servers.Remove(data);
			SaveConfig();
		}

		private bool LoadConfig()
		{
			string configPath = Path.Combine(m_configPath, "config.xml");

			m_servers = new ObservableCollection<ServerData>();

			if (!File.Exists(configPath))
			{
				return true;
			}

			try
			{
				XmlSerializer serializer = new XmlSerializer(typeof(SystemConfig));
				using (XmlReader reader = XmlReader.Create(configPath))
				{
					SystemConfig config = (SystemConfig)serializer.Deserialize(reader);

					foreach (ServerConfig serverConfig in config.Servers)
					{
						AddServerInternal(serverConfig);
					}
				}

				return true;
			}
			catch (Exception e)
			{
				Log.Error("conct", e.GetType().Name + ": " +  e.Message);
			}

			return false;
		}

		private void SaveConfig()
		{
			string configPath = Path.Combine(m_configPath, "config.xml");

			try
			{
				XmlSerializer serializer = new XmlSerializer(typeof(SystemConfig));
				using (XmlWriter writer = XmlWriter.Create(configPath))
				{
					SystemConfig config = new SystemConfig();
					config.Servers = m_servers.Select(s => s.Config).ToArray();

					serializer.Serialize(writer, config);
				}
			}
			catch (Exception e)
			{
				Log.Error("conct", e.GetType().Name + ": " + e.Message);
			}
		}

		private void SyncDevices()
		{
			m_devices.Sync(m_device.ConnectedDevices, (s, t) => t.Address == s, s => new ConnectedDevice(s));
		}
	}
}