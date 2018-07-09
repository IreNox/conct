using System;
using System.Drawing;

namespace conct
{
	public class ServerViewModel
	{
		private ServerData m_data;

		public ServerViewModel(ServerData data)
		{
			m_data = data;
		}

		public Color Color
		{
			get
			{
				int value = Title.GetHashCode();
				int red = (value >> 0) & 0xff;
				int green = (value >> 8) & 0xff;
				int blue = (value >> 16) & 0xff;
				return Color.FromArgb(red, green, blue);
			}
		}

		public string Title
		{
			get
			{
				string title = m_data.Config.Title;
				string host = m_data.Config.Hostname + ":" + m_data.Config.HostPort.ToString();
				if (String.IsNullOrEmpty(title))
				{
					return host;
				}
				else
				{
					return title + "(" + host + ")";
				}
			}
		}

		public string Error
		{
			get
			{
				if(m_data.Handle == IntPtr.Zero)
				{
					return "Could not connect";
				}

				return String.Empty;
			}
		}

		public ServerData Data
		{
			get { return m_data; }
		}
	}
}
