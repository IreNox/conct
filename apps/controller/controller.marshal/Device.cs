using System;

namespace conct
{
	public class DeviceAddress
	{
		private byte[] m_address;

		public DeviceAddress()
		{
		}
	}

	public class Device
	{
		private string m_name;
		private DeviceAddress m_address;
		private Guid m_guid;

		Device()
		{

		}
	}
}
