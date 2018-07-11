using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;

using Xamarin.Forms;

namespace conct
{
    public partial class DeviceInstanceControl : ContentView
	{
		private DeviceInstance m_instance;
		private BidirectionalDictionary<DeviceInstanceProperty, View> m_propertyControlMapping;

		private bool m_disableApplyValue;

        public DeviceInstanceControl(DeviceInstance instance)
        {
			m_instance = instance;
			m_propertyControlMapping = new BidirectionalDictionary<DeviceInstanceProperty, View>();

			InitializeComponent();

			BindingContext = new DeviceInstanceViewModel(instance);

			BuildUI();

			m_instance.PropertyChanged += Instance_PropertyChanged;
        }

		public DeviceInstance Instance
		{
			get { return m_instance; }
		}

		private void Instance_PropertyChanged(DeviceInstance instance, DeviceInstanceProperty property)
		{
			if (!m_propertyControlMapping.Contains1(property))
			{
				return;
			}

			FillControlFromProperty(property, m_propertyControlMapping.Forward[property]);
		}

		private void BooleanControl_Toggled(object sender, ToggledEventArgs e)
		{
			if (m_disableApplyValue)
			{
				return;
			}

			View control = (View)sender;
			DeviceInstanceProperty property = m_propertyControlMapping.Reverse[control];

			Value value = new Value();
			value.Boolean = e.Value;

			property.Value = value;
		}

		private void EntryControl_TextChanged(object sender, TextChangedEventArgs e)
		{
			if (m_disableApplyValue)
			{
				return;
			}

			View control = (View)sender;
			DeviceInstanceProperty property = m_propertyControlMapping.Reverse[control];

			Value value = new Value();
			switch (property.Property.Type.ValueType)
			{
				case ValueType.Integer:
					value.Integer = Int32.Parse(e.NewTextValue);
					break;

				case ValueType.Unsigned:
					value.Unsigned = UInt32.Parse(e.NewTextValue);
					break;

				case ValueType.String:
					value.String = e.NewTextValue;
					break;
			}

			property.Value = value;
		}

		private void PercentControl_ValueChanged(object sender, ValueChangedEventArgs e)
		{
			if (m_disableApplyValue)
			{
				return;
			}

			View control = (View)sender;
			DeviceInstanceProperty property = m_propertyControlMapping.Reverse[control];

			Value value = new Value();
			value.PercentValue = (UInt16)e.NewValue;

			property.Value = value;
		}

		private void BuildUI()
		{
			InterfaceType type = m_instance.Type;

			Grid gridLayout = new Grid()
			{
				ColumnDefinitions =
					{
						new ColumnDefinition() { Width = new GridLength(0.1, GridUnitType.Star) },
						new ColumnDefinition() { Width = new GridLength(1.0, GridUnitType.Star) }
					}
			};

			int row = 0;
			foreach (DeviceInstanceProperty property in m_instance.Properties)
			{
				View control = null;
				switch(property.Property.Type.ValueType)
				{
					case ValueType.Boolean:
						Switch booleanControl = new Switch();
						booleanControl.Toggled += BooleanControl_Toggled;
						control = booleanControl;
						break;

					case ValueType.Integer:
						Entry integerControl = new Entry()
						{
							Behaviors = { new NumericValidationBehavior() }
						};
						integerControl.TextChanged += EntryControl_TextChanged;
						control = integerControl;
						break;

					case ValueType.Unsigned:
						Entry unsignedControl = new Entry()
						{
							Behaviors = { new NumericValidationBehavior() }
						};
						unsignedControl.TextChanged += EntryControl_TextChanged;
						control = unsignedControl;
						break;

					case ValueType.PercentValue:
						Slider percentControl = new Slider()
						{
							Minimum = 0.0,
							Maximum = 65535.0,
							Value = 0.0
						};
						percentControl.ValueChanged += PercentControl_ValueChanged;
						control = percentControl;
						break;

					case ValueType.String:
						Entry stringControl = new Entry();
						stringControl.TextChanged += EntryControl_TextChanged;
						control = stringControl;
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
					Text = property.Property.Name
				};

				gridLayout.Children.Add(nameLabel, 0, row);
				gridLayout.Children.Add(control, 1, row);
				row++;

				FillControlFromProperty(property, control);
				m_propertyControlMapping.Add(property, control);
			}

			BaseLayout.Children.Add(gridLayout);
		}

		private void FillControlFromProperty(DeviceInstanceProperty property, View control)
		{
			Value value = property.Value;
			if (value == null)
			{
				return;
			}

			m_disableApplyValue = true;
			switch (property.Property.Type.ValueType)
			{
				case ValueType.Boolean:
					Switch booleanControl = (Switch)control;
					booleanControl.IsToggled = value.Boolean;
					break;

				case ValueType.Integer:
					Entry integerControl = (Entry)control;
					integerControl.Text = value.Integer.ToString();
					break;

				case ValueType.Unsigned:
					Entry unsignedControl = (Entry)control;
					unsignedControl.Text = value.Unsigned.ToString();
					break;

				case ValueType.PercentValue:
					Slider percentControl = (Slider)control;
					percentControl.Value = value.PercentValue;
					break;

				case ValueType.String:
					Entry stringControl = (Entry)control;
					stringControl.Text = value.String;
					break;

				default:
					break;
			}

			control.IsEnabled = property.Property.HasSetter;
			m_disableApplyValue = false;
		}
	}
}
