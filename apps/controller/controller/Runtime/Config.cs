using System;

namespace conct
{
	public struct ServerConfig
	{
		public string Title;
		public string Hostname;
		public UInt16 HostPort;
	}

	public struct SystemConfig
	{
		public ServerConfig[] Servers;
	}

	public class ServerData
	{
		public ServerConfig Config;
		public IntPtr Handle;
	}
}