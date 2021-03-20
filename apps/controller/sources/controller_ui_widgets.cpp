#include "controller_ui_widgets.h"

#include "conct_core.h"

#include <imapp/imapp.h>

namespace conct
{
	UiWidget::UiWidget( nk_context* _pNkContext )
	 : pNkContext( _pNkContext )
	{
	}

	UiWidget::~UiWidget()
	{
	}

	UiHeader::UiHeader( nk_context* pNkContext, const char* pId )
		: UiWidget( pNkContext )
	{
		nk_style_push_vec2( pNkContext, &pNkContext->style.window.group_padding, nk_vec2( pNkContext->style.window.header.padding.x, 1.0f ) );
		nk_style_push_style_item( pNkContext, &pNkContext->style.window.fixed_background, pNkContext->style.window.header.normal );
		isVisible = nk_group_begin( pNkContext, pId, NK_WINDOW_NO_SCROLLBAR );
	}

	UiHeader::~UiHeader()
	{
		if( isVisible )
		{
			nk_group_end( pNkContext );
		}
		nk_style_pop_style_item( pNkContext );
		nk_style_pop_vec2( pNkContext );
	}

	void UI::doHeaderLabel( nk_context* pNkContext, const char* pText )
	{
		UiHeader header( pNkContext, pText );
		if( !header.isVisible )
		{
			return;
		}

		nk_label( pNkContext, pText, NK_TEXT_LEFT );
	}

	void UI::doHeaderImageLabel( nk_context* pNkContext, const struct nk_image& image, const char* pText )
	{
		UiHeader header( pNkContext, pText );
		if( !header.isVisible )
		{
			return;
		}
		
		const float width = nk_window_get_width( pNkContext );

		nk_layout_row_begin( pNkContext, NK_STATIC, 0.0f, 3 );

		const float imageWidth = pNkContext->style.window.header.padding.x + float( image.w ) + pNkContext->style.window.spacing.x;
		nk_layout_row_push( pNkContext, 24.0f );
		nk_style_push_vec2( pNkContext, &pNkContext->style.window.spacing, nk_vec2( 0.0f, 9.0f ) );
		nk_image( pNkContext, image );
		nk_style_pop_vec2( pNkContext );

		const float remainingWidth = width - imageWidth;
		nk_layout_row_push( pNkContext, remainingWidth );
		nk_label( pNkContext, pText, NK_TEXT_LEFT );
	}
}