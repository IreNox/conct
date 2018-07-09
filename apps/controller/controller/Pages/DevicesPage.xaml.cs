using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace conct
{
	public partial class DevicesPage : ContentPage
	{
		public DevicesPage()
		{
			InitializeComponent();

			this.BindingContext = new DevicesViewModel();
		}
	}
}
