#pragma once
#include "runtime\function\framework\component\animation\animation_clip.h"

namespace QYHS
{
	template<>
	Json Serializer::write(const AnimationNodeMap&instance);
	template<>
	AnimationNodeMap & Serializer::read(const Json & json_context,AnimationNodeMap & instance);
	template<>
	Json Serializer::write(const AnimationChannel&instance);
	template<>
	AnimationChannel & Serializer::read(const Json & json_context,AnimationChannel & instance);
	template<>
	Json Serializer::write(const AnimationClip&instance);
	template<>
	AnimationClip & Serializer::read(const Json & json_context,AnimationClip & instance);
	template<>
	Json Serializer::write(const AnimationAsset&instance);
	template<>
	AnimationAsset & Serializer::read(const Json & json_context,AnimationAsset & instance);
}//namespace

