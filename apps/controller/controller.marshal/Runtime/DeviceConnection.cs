using System;
using System.Runtime.InteropServices;

namespace conct
{
	[StructLayout(LayoutKind.Sequential)]
	public class DeviceConnection
	{
		public Byte Status;
		public Byte DeviceId;
	}
}
