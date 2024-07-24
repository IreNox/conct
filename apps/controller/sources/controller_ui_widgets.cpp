#include "controller_ui_widgets.h"

#include "conct_core.h"

#include <imapp/imapp.h>

namespace conct
{
	UiHeader::UiHeader( UiToolboxWindow& window, const char* name )
		: UiWidget( window )
	{
		//nk_style_push_vec2( pNkContext, &pNkContext->style.window.group_padding, nk_vec2( pNkContext->style.window.header.padding.x, 1.0f ) );
		//nk_style_push_style_item( pNkContext, &pNkContext->style.window.fixed_background, pNkContext->style.window.header.normal );
		//isVisible = nk_group_begin( pNkContext, name, NK_WINDOW_NO_SCROLLBAR );
	}

	UiHeader::~UiHeader()
	{
		if( isVisible )
		{
			//nk_group_end( pNkContext );
		}
		//nk_style_pop_style_item( pNkContext );
		//nk_style_pop_vec2( pNkContext );
	}

	void ui::doHeaderLabel( UiToolboxWindow& window, const char* text )
	{
		UiHeader header( window, text );
		if( !header.isVisible )
		{
			return;
		}

		window.label( text );
	}

	void ui::doHeaderImageLabel( UiToolboxWindow& window, const ImUiImage& image, const char* text )
	{
		UiHeader header( window, text );
		if( !header.isVisible )
		{
			return;
		}

		const float width = window.getRect().size.width;

		UiWidgetLayoutHorizontal layout( window, 5.0f );

		//nk_layout_row_begin( pNkContext, NK_STATIC, 0.0f, 3 );

		//const float imageWidth = pNkContext->style.window.header.padding.x + float( image.w ) + pNkContext->style.window.spacing.x;
		//nk_style_push_vec2( pNkContext, &pNkContext->style.window.spacing, nk_vec2( 0.0f, 9.0f ) );
		window.image( image );
		//nk_style_pop_vec2( pNkContext );

		//const float remainingWidth = width - imageWidth;
		//nk_layout_row_push( pNkContext, remainingWidth );
		window.label( text );
	}
}