#pragma once
#include <vector>
#include "animation_clip.h"
#include "animation_skeleton_map.h"
namespace QYHS
{
	class BoneBlendWeight
	{
	public:
		std::vector<float> blend_weights;
	};

	class BlendStateWithClipData
	{
	public:
		std::vector<AnimationClip>		  m_animation_clips;
		std::vector<AnimationSkeletonMap> m_animation_skeleton_maps;
		std::vector<BoneBlendWeight>      blend_weights;
		size_t clip_count;
		std::vector<float> blend_ratios;
	private:
	};
}