#pragma once
#include "function/platform/platform.h"
#include <cstdint>
#include "core/math/directx_math.h"
namespace qyhs
{
	struct Canvas
	{
	public:
		uint32_t width = 0;
		uint32_t height = 0;
		float dpi{ 0 };
		inline void init(platform::WindowProperties window_properties)
		{
			width = window_properties.width;
			height = window_properties.height;
			dpi = window_properties.dpi;
		}

		inline void init(const Canvas& canvas)
		{
			*this = canvas;
		}

		inline void init(platform::WindowType window)
		{
			platform::WindowProperties window_properties;
			platform::getWindowProperties(window, &window_properties);
			init(window_properties);
		}
		inline float getLogicalWidth()const
		{
			return physicalToLogical(getPhysicalWidth());
		}
		inline float getLogicalHeight()const
		{
			return physicalToLogical(getPhysicalHeight());
		}
		inline float const physicalToLogical(uint32_t physical)const
		{
			return (float)physical / getDPIScaling();
		}
		inline float const logicalToPhysical(float logical) const
		{
			return uint32_t(logical * getDPIScaling());
		}
		inline float getDPIScaling()const
		{
			return (float)dpi / 96.0f;					//96.0f for window api,  1.0 means that 1 logical unit qual 1px ,2.0 means that 1 logical unit qual 2px
		}
		inline uint32_t getPhysicalWidth() const { return this->width; }
		inline uint32_t getPhysicalHeight() const { return this->height; }
		inline XMMATRIX getProjection()const
		{
			return XMMatrixOrthographicOffCenterLH(0.0, (float)getLogicalWidth(), (float)getLogicalHeight(), 0.0, -1.0, 1.0);
		}
	
	};
}