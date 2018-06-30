using System;
using System.Runtime.InteropServices;

namespace conct
{
	[StructLayout(LayoutKind.Sequential)]
	public class Instance
	{
		public UInt16 InstanceId;
		public UInt16 TypeCrc;
	}
}