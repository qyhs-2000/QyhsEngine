#pragma once
#include "core/meta/reflection/reflection.h"
#include "core/math/vector3.h"
#include <vector>
namespace qyhs
{
	REFLECTION_TYPE(AnimationNodeMap)
		CLASS(AnimationNodeMap,Fields)
	{
		REFLECTION_BODY(AnimationNodeMap)
	public:
		std::vector<std::string> convert;
	private:
	};

	REFLECTION_TYPE(AnimationChannel)
		CLASS(AnimationChannel,Fields)
	{
		REFLECTION_BODY(AnimationChannel)
	public:
		std::string name;
		std::vector<Vector3> position_keys;
		std::vector<Quaternion> rotation_keys;
		std::vector<Vector3> scaling_keys;
	private:
	};

	REFLECTION_TYPE(AnimationClip)
	CLASS(AnimationClip,Fields)
	{
		REFLECTION_BODY(AnimationClip)
	public:
		int total_frame;
		int node_count;
		std::vector<AnimationChannel> node_channels;
	private:
	};

	REFLECTION_TYPE(AnimationAsset)
		CLASS(AnimationAsset,Fields)
	{
		REFLECTION_BODY(AnimationAsset)
	public:
		AnimationNodeMap node_map;
		AnimationClip clip_data;
		std::string skeleton_file_path;
	private:
	};
}