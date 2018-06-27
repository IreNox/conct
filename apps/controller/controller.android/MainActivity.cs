using System;

using Android.App;
using Android.Content.PM;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using Android.OS;
using System.IO;

namespace conct
{
    [Activity(Label = "controller", Icon = "@mipmap/icon", Theme = "@style/MainTheme", MainLauncher = true, ConfigurationChanges = ConfigChanges.ScreenSize | ConfigChanges.Orientation)]
    public class MainActivity : global::Xamarin.Forms.Platform.Android.FormsAppCompatActivity
    {
		private TypeCollection m_typeCollection;

        protected override void OnCreate(Bundle bundle)
        {
			string pName = ApplicationContext.PackageName;
			ApplicationInfo appInfo = ApplicationContext.PackageManager.GetApplicationInfo(pName, 0);
			string path2 = appInfo.SourceDir;
			string path = Path.Combine(path2, "assets/device.xml");
			var fgfhg = Directory.GetFiles(path2);
			var xx = Directory.GetDirectories(path2);

			string bla = File.ReadAllText(path);

			TabLayoutResource = Resource.Layout.Tabbar;
            ToolbarResource = Resource.Layout.Toolbar;

            base.OnCreate(bundle);

            global::Xamarin.Forms.Forms.Init(this, bundle);
            LoadApplication(new App());

			m_typeCollection = new TypeCollection();
        }
    }
}

