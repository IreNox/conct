using System;

namespace conct
{
	public delegate void Event<TSender, TEventArgs>(TSender sender, TEventArgs args);

	public static class DynamicLibrary
	{
		internal const string Name = "controller.native.desktop.dll";
	}
}