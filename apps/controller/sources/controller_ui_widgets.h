#pragma once

struct nk_context;
struct nk_image;

namespace conct
{
	struct UiWidget
	{
					UiWidget( nk_context* pNkContext );
					~UiWidget();

		nk_context*	pNkContext;
	};

	struct UiHeader : UiWidget
	{
					UiHeader( nk_context* pNkContext, const char* pId );
					~UiHeader();

		bool		isVisible;
	};

	struct UI
	{
		static void doHeaderLabel( nk_context* pNkContext, const char* pText );
		static void	doHeaderImageLabel( nk_context* pNkContext, const struct nk_image& image, const char* pText );
	};
}