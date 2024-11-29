#pragma once
#include <memory>
#include <string>
#include "animation_clip.h"
#include "animation_skeleton_map.h"
#include "animation_mask.h"
#include "resource/type/data/skeleton_data.h"
namespace QYHS
{
	class AnimationLoader
	{
	public:
		static std::shared_ptr<AnimationClip> loadAnimationClipData(std::string clip_data_path);
		static std::shared_ptr<AnimationSkeletonMap> loadAnimationSkeletonMap(std::string clip_data_path);
		static std::shared_ptr<BoneBlendMask> loadSkeletonMask(std::string clip_data_path);
		static std::shared_ptr<SkeletonData> loadSkeletonData(std::string skeleton_data_path);
	};
}