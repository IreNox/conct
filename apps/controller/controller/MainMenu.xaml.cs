using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace conct
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class MainMenu : ContentPage
    {
        public ListView ListView;

        public MainMenu()
        {
            InitializeComponent();

            BindingContext = new MainMenuViewModel();
            ListView = MenuItemsListView;
        }

        class MainMenuViewModel : INotifyPropertyChanged
        {
            public ObservableCollection<MainMenuItem> MenuItems { get; set; }

            public MainMenuViewModel()
            {
                MenuItems = new ObservableCollection<MainMenuItem>(new[]
                {
                    new MainMenuItem() { Image = "icon_house.png", Title = "Home", TargetType=typeof(HomePage) },
                    new MainMenuItem() { Image = "icon_settings.png", Title = "Settings", TargetType=typeof(SettingsPage) },
					new MainMenuItem() { Image = "icon_chat.png", Title = "Credits", TargetType=typeof(CreditsPage) },
                });
            }

            public event PropertyChangedEventHandler PropertyChanged;
            void OnPropertyChanged([CallerMemberName] string propertyName = "")
            {
				if (PropertyChanged == null)
				{
					return;
				}

                PropertyChanged.Invoke(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}