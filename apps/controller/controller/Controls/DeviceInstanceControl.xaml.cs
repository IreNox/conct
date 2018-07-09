using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;

using Xamarin.Forms;

namespace conct
{
    public sealed partial class DeviceInstanceControl : StackLayout
    {
		private DeviceInstance m_instance;

        public DeviceInstanceControl(DeviceInstance instance)
        {
			m_instance = instance;

            InitializeComponent();

			BindingContext = new DeviceInstanceViewModel(instance);
        }

		public DeviceInstance Instance
		{
			get { return m_instance; }
		}

		private void BuildUI()
		{
			InterfaceType type = m_instance.Type;

			foreach (InterfaceProperty prop in type.Properties)
			{
				View control = null;
				switch(prop.Type.ValueType)
				{
					case ValueType.Boolean:
						control = new Switch();
						break;

					case ValueType.Integer:
						control = new Entry()
						{
							Behaviors = { new NumericValidationBehavior() }
						};
						break;

					case ValueType.Unsigned:
						control = new Entry()
						{
							Behaviors = { new NumericValidationBehavior() }
						};
						break;

					case ValueType.PercentValue:
						control = new Slider()
						{
							Minimum = 0.0,
							Maximum = 65535.0,
							Value = 0.0
						};
						break;

					case ValueType.String:
						control = new Entry();
						break;

					default:
						break;
				}

				if (control == null)
				{
					continue;
				}

				control.IsEnabled = false;

				Label nameLabel = new Label()
				{
					Text = prop.Name
				};

				StackLayout propLayout = new StackLayout()
				{
					Orientation = StackOrientation.Horizontal,
					Children = { nameLabel, control }
				};

				Children.Add(propLayout);
			}
		}
    }
}
