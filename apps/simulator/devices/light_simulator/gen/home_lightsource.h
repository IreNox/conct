#pragma once

namespace conct
{
	static constexpr TypeCrc LightSourceTypeCrc = 0x671e;

	enum LightSource
	{
		LightSource_W,
		LightSource_WC,
		LightSource_RGB,
		LightSource_RGBW,
		LightSource_RGBWC,
	};
}
