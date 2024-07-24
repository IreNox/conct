#pragma once

namespace imui
{
	class UiWindow;
}

namespace conct
{
	using namespace imui;

	struct UiHeader : UiWidget
	{
					UiHeader( UiToolboxWindow& window, const char* name );
					~UiHeader();

		bool		isVisible;
	};

	namespace ui
	{
		void		doHeaderLabel( UiToolboxWindow& window, const char* text );
		void		doHeaderImageLabel( UiToolboxWindow& window, const ImUiImage& image, const char* text );
	};
}