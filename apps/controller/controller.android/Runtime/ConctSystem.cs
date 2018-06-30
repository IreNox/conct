using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;

namespace conct
{
	public class ConctSystem : IDisposable
	{
		private Device m_device;
		private Controller m_controller;

		private TypeCollection m_typeCollection;

		private List<ServerConfig> m_servers;

		public ConctSystem()
		{
			m_device = new Device();
			m_controller = m_device.Controller;
			m_typeCollection = new TypeCollection();
			m_servers = new List<ServerConfig>();
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
			//m_device.AddPort()
			bool result = true;
			result &= m_typeCollection.Load(path);

			return result;
		}
	}
}