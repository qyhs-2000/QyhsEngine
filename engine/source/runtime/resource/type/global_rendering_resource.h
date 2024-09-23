#pragma once

#include "core/meta/reflection/reflection.h"
#include "data/camera_config.h"
namespace QYHS
{
	REFLECTION_TYPE(GlobalRenderConfig)
		CLASS(GlobalRenderConfig, Fields)
	{
		REFLECTION_BODY(GlobalRenderConfig)
	public:
		CameraConfig camera_config;
	private:

	};
}