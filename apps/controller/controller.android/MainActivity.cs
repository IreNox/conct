using System;
using System.IO;

using Android.App;
using Android.Content.PM;
using Android.OS;

namespace conct
{
    [Activity(Label = "controller", Icon = "@mipmap/icon", Theme = "@style/MainTheme", MainLauncher = true, ConfigurationChanges = ConfigChanges.ScreenSize | ConfigChanges.Orientation)]
    public class MainActivity : global::Xamarin.Forms.Platform.Android.FormsAppCompatActivity
    {
		private ConctSystem m_system;

        protected override void OnCreate(Bundle bundle)
        {
			m_system = new ConctSystem();

			TabLayoutResource = Resource.Layout.Tabbar;
			ToolbarResource = Resource.Layout.Toolbar;

            base.OnCreate(bundle);

            global::Xamarin.Forms.Forms.Init(this, bundle);
			global::Plugin.CrossPlatformTintedImage.Android.TintedImageRenderer.Init();
			LoadApplication(new App(m_system));

			(string configPath, string typesPath) = MoveAssets();
			if (!m_system.Load(configPath, typesPath))
			{
				App.Current.MainPage.DisplayAlert("conct.controller", "Failed to load configuration!", "Close");
			}
		}

		private (string, string) MoveAssets()
		{
			string appDataPath = System.Environment.GetFolderPath(System.Environment.SpecialFolder.LocalApplicationData);

			string configPath = Path.Combine(appDataPath, "config");
			string typesPath = Path.Combine(appDataPath, "types");
			Directory.CreateDirectory(configPath);
			Directory.CreateDirectory(typesPath);

			string[] pathes = new string[] { "core", "home" };
			foreach (string path in pathes)
			{
				string targetPath = Path.Combine(typesPath, path);
				Directory.CreateDirectory(targetPath);

				string[] files = Assets.List(path);

				foreach (string asset in files)
				{
					string assetFile = Path.Combine(path, asset);
					string targetFile = Path.Combine(targetPath, asset);
					Stream assetStream = Assets.Open(assetFile);
					string assetData = new StreamReader(assetStream).ReadToEnd();
					File.WriteAllText(targetFile, assetData);
				}
			}

			return (configPath, typesPath);
		}
	}
}

