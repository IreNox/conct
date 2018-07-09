using System;
using System.Timers;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

[assembly: XamlCompilation(XamlCompilationOptions.Compile)]
namespace conct
{
	public partial class App : Application
	{
		private Conct m_system;
		private Timer m_timer;

		public App(Conct system)
		{
			m_system = system;

			InitializeComponent();
			MainPage = new Main();

			m_timer = new Timer(15.0);
			m_timer.Elapsed += Timer_Elapsed;
		}

		private void Timer_Elapsed(object sender, ElapsedEventArgs e)
		{
			Xamarin.Forms.Device.BeginInvokeOnMainThread(() => m_system.Loop());
		}

		protected override void OnStart()
		{
			m_timer.Start();
		}

		protected override void OnSleep()
		{
			m_timer.Stop();
		}

		protected override void OnResume()
		{
			m_timer.Start();
		}

		public static Conct System
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
