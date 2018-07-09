using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using Xamarin.Forms;
using Xamarin.Forms.Platform.WPF;

namespace conct
{
	public partial class MainWindow : FormsApplicationPage
	{
		private Conct m_system;

		public MainWindow()
		{
			m_system = new Conct();

			InitializeComponent();

			Forms.Init();
			LoadApplication(new App(m_system));

			DirectoryInfo executableDir = Directory.GetParent(Assembly.GetExecutingAssembly().Location);
			string configPath = executableDir.ToString();
			string typesPath = System.IO.Path.Combine(executableDir.Parent.Parent.Parent.ToString(), "config/types");
			m_system.Load(configPath, typesPath);
		}
	}
}
