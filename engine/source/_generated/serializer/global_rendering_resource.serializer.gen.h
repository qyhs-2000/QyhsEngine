#pragma once
#include "runtime\resource\type\global_rendering_resource.h"

namespace QYHS
{
	template<>
	Json Serializer::write(const SkyBoxSpecularMap&instance);
	template<>
	SkyBoxSpecularMap & Serializer::read(const Json & json_context,SkyBoxSpecularMap & instance);
	template<>
	Json Serializer::write(const GlobalRenderConfig&instance);
	template<>
	GlobalRenderConfig & Serializer::read(const Json & json_context,GlobalRenderConfig & instance);
}//namespace

