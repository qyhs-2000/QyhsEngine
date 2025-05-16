#pragma once
#include "blend_state.h"
#include <string>
#include "animation.h"
#include "animation_skeleton_map.h"
#include "animation_mask.h"
#include "resource/type/data/skeleton_data.h"
#include <map>
#include <memory>
#include <string>
namespace qyhs
{
	class BlendState;
	class AnimationManager
	{
	public:
		static BlendStateWithClipData getBlendStateWithClipData (BlendState blend_state);
		static std::shared_ptr<AnimationClip> tryLoadAnimation(std::string clip_file_path);
		static std::shared_ptr<AnimationSkeletonMap> tryLoadAnimationSkeletonMap(std::string skeleton_map_url);
		static std::shared_ptr<BoneBlendMask> tryLoadSkeletonMask(std::string skeleton_map_url);
		static std::shared_ptr<SkeletonData> tryLoadSkeletonData(std::string skeleton_data_url);
	private:
		static std::map<std::string, std::shared_ptr<AnimationClip>> m_animation_clip_cache;
		static std::map < std::string, std::shared_ptr<AnimationSkeletonMap>> m_animation_skeleton_map_cache;
		static std::map<std::string, std::shared_ptr<BoneBlendMask>> m_skeleton_mask_cache;
		static std::map < std::string, std::shared_ptr<SkeletonData>> m_skeleton_definition_cache;
	};
}