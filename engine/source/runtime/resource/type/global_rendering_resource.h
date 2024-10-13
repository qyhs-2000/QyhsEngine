#pragma once

#include "core/meta/reflection/reflection.h"
#include "data/camera_config.h"
namespace QYHS
{
	REFLECTION_TYPE(SkyBoxSpecularMap)
		CLASS(SkyBoxSpecularMap, Fields)
	{
		REFLECTION_BODY(SkyBoxSpecularMap)
	public:
		std::string m_positive_x_map;
		std::string m_negative_x_map;
		std::string m_positive_y_map;
		std::string m_negative_y_map;
		std::string m_positive_z_map;
		std::string m_negative_z_map;

	private:
	};

	REFLECTION_TYPE(GlobalRenderConfig)
		CLASS(GlobalRenderConfig, Fields)
	{
		REFLECTION_BODY(GlobalRenderConfig)
	public:
		CameraConfig camera_config;
		SkyBoxSpecularMap m_skybox_specular_map;
	private:

	};

}