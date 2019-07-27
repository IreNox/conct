using System;

namespace conct
{
	public delegate void Event<TSender, TEventArgs>(TSender sender, TEventArgs args);
}