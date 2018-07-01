using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

using Xamarin.Forms;

namespace conct
{
	public class NumericValidationBehavior : Behavior<Entry>
	{
		protected override void OnAttachedTo(Entry entry)
		{
			entry.TextChanged += OnEntryTextChanged;
			base.OnAttachedTo(entry);
		}

		protected override void OnDetachingFrom(Entry entry)
		{
			entry.TextChanged -= OnEntryTextChanged;
			base.OnDetachingFrom(entry);
		}

		private static void OnEntryTextChanged(object sender, TextChangedEventArgs args)
		{
			if (!string.IsNullOrWhiteSpace(args.NewTextValue))
			{
				((Entry)sender).Text = new String(args.NewTextValue.Where(c => char.IsDigit(c)).ToArray());
			}
		}
	}
}
