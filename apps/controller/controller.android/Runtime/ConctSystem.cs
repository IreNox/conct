using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Serialization;
using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using Android.Util;

namespace conct
{
	public class ConctSystem : IDisposable
	{
		private Device m_device;
		private Controller m_controller;

		private TypeCollection m_typeCollection;

		private string m_dataPath;
		private List<ServerData> m_servers;

		public ConctSystem()
		{
			m_device = new Device();
			m_controller = m_device.Controller;
			m_typeCollection = new TypeCollection();
		}

		public void Dispose()
		{
			m_controller = null;

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

		public bool Load(string path)
		{
			m_dataPath = path;

			bool result = true;
			result &= LoadConfig(path);
			result &= m_typeCollection.Load(path);

			return result;
		}

		public IEnumerable<ServerData> Servers
		{
			get { return m_servers; }
		}

		public ServerData AddServer(ServerConfig config)
		{
			ServerData data = AddServerInternal(config);
			SaveConfig();

			return data;
		}

		private ServerData AddServerInternal(ServerConfig config)
		{
			ServerData data = new ServerData()
			{
				Config = config,
				Handle = IntPtr.Zero
			};

			data.Handle = m_device.AddPort(config.Hostname, config.HostPort);

			m_servers.Add(data);
			return data;
		}

		public void RemoveServer(ServerData data)
		{
			m_device.RemovePort(data.Handle);

			m_servers.Remove(data);
			SaveConfig();
		}

		private bool LoadConfig(string path)
		{
			string configPath = Path.Combine(path, "config.xml");

			m_servers = new List<ServerData>();

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
			string configPath = Path.Combine(m_dataPath, "config.xml");

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
	}
}