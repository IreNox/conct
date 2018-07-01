using System;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

[assembly: XamlCompilation(XamlCompilationOptions.Compile)]
namespace conct
{
	public partial class App : Application
	{
		private ConctSystem m_system;

		public App(ConctSystem system)
		{
			m_system = system;

			InitializeComponent();

			MainPage = new Main();
		}

		protected override void OnStart ()
		{
			// Handle when your app starts
		}

		protected override void OnSleep ()
		{
			// Handle when your app sleeps
		}

		protected override void OnResume ()
		{
			// Handle when your app resumes
		}

		public static ConctSystem System
		{
			get
			{
				App app = Application.Current as App;
				if (app == null)
				{
					return null;
				}

				return app.m_system;
			}
		}
	}
}
