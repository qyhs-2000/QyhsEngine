#include "animation_loader.h"
#include "function/global/global_context.h"
#include "resource/asset_manager/asset_manager.h"

namespace QYHS
{
	std::shared_ptr<AnimationClip> QYHS::AnimationLoader::loadAnimationClipData(std::string clip_data_path)
	{
		AnimationAsset animation_clip;
		g_runtime_global_context.m_asset_manager->loadAsset(clip_data_path, animation_clip);
		return std::make_shared<AnimationClip>(animation_clip.clip_data);
	}

	std::shared_ptr<AnimationSkeletonMap> AnimationLoader::loadAnimationSkeletonMap(std::string clip_data_path)
	{
		AnimationSkeletonMap skeleton_map;
		g_runtime_global_context.m_asset_manager->loadAsset(clip_data_path, skeleton_map);
		return std::make_shared<AnimationSkeletonMap>(skeleton_map);
	}

	std::shared_ptr<BoneBlendMask> AnimationLoader::loadSkeletonMask(std::string clip_data_path)
	{
		BoneBlendMask animation_mask;
		g_runtime_global_context.m_asset_manager->loadAsset(clip_data_path, animation_mask);
		return std::make_shared<BoneBlendMask>(animation_mask);
	}

	std::shared_ptr<SkeletonData> AnimationLoader::loadSkeletonData(std::string skeleton_data_path)
	{
		SkeletonData skeleton_data;
		g_runtime_global_context.m_asset_manager->loadAsset(skeleton_data_path, skeleton_data);
		return std::make_shared<SkeletonData>(skeleton_data);
	}

}
