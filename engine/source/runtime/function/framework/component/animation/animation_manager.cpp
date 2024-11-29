#include "animation_manager.h"
#include "animation_component.h"
#include "animation_loader.h"
namespace QYHS
{
	std::map<std::string, std::shared_ptr<AnimationClip>> AnimationManager::m_animation_clip_cache;
	std::map<std::string, std::shared_ptr<AnimationSkeletonMap>> AnimationManager::m_animation_skeleton_map_cache;
	std::map<std::string, std::shared_ptr<BoneBlendMask>> AnimationManager::m_skeleton_mask_cache;
	std::map < std::string, std::shared_ptr<SkeletonData>> AnimationManager::m_skeleton_definition_cache;

	BlendStateWithClipData AnimationManager::getBlendStateWithClipData(BlendState blend_state)
	{
		BlendStateWithClipData blend_state_with_clip_data;
		for (auto animation_file_path : blend_state.blend_clip_file_path)
		{
			tryLoadAnimation(animation_file_path);
		}
		for (auto anim_skeleton_map_file : blend_state.blend_anim_skel_map_path)
		{
			tryLoadAnimationSkeletonMap(anim_skeleton_map_file);
		}
		for (auto skeleton_mast_path : blend_state.blend_mask_file_path)
		{
			tryLoadSkeletonMask(skeleton_mast_path);
		}
		blend_state_with_clip_data.clip_count = blend_state.clip_count;
		blend_state_with_clip_data.blend_ratios = blend_state.blend_ratio;
		for (const std::string& iter : blend_state.blend_clip_file_path)
		{
			blend_state_with_clip_data.m_animation_clips.push_back(*m_animation_clip_cache[iter]);
		}
		for (const std::string& iter : blend_state.blend_anim_skel_map_path)
		{
			blend_state_with_clip_data.m_animation_skeleton_maps.push_back(*m_animation_skeleton_map_cache[iter]);
		}
		std::vector<std::shared_ptr<BoneBlendMask>> blend_masks;
		for (std::string  iter : blend_state.blend_mask_file_path)
		{
			blend_masks.push_back(m_skeleton_mask_cache[iter]);
		}
		size_t skeleton_bone_count = m_skeleton_definition_cache[blend_masks[0]->skeleton_file_path]->bones_map.size();
		blend_state_with_clip_data.blend_weights.resize(blend_state.clip_count);
		for (int clip_index = 0; clip_index < blend_state.clip_count; ++clip_index)
		{
			blend_state_with_clip_data.blend_weights[clip_index].blend_weights.resize(skeleton_bone_count);
		}
		for (size_t bone_index = 0; bone_index < skeleton_bone_count; ++bone_index)
		{
			float sum_weight = 0;
			for (size_t clip_index = 0; clip_index < blend_state.clip_count; ++clip_index)
			{
				if (blend_masks[clip_index]->enabled[bone_index])
				{
					sum_weight += blend_state.blend_weight[clip_index];
				}
			}
			for (size_t clip_index = 0; clip_index < blend_state.clip_count; ++clip_index)
			{
				if (blend_masks[clip_index]->enabled[bone_index])
				{
					blend_state_with_clip_data.blend_weights[clip_index].blend_weights[bone_index] = blend_state.blend_weight[clip_index] / sum_weight;
				}
				else
				{
					blend_state_with_clip_data.blend_weights[clip_index].blend_weights[bone_index] = 0;
				}
			}
		}
		return blend_state_with_clip_data;
	}

	std::shared_ptr<AnimationClip> AnimationManager::tryLoadAnimation(std::string clip_file_path)
	{
		std::shared_ptr<AnimationClip> result;
		auto iter = m_animation_clip_cache.find(clip_file_path);
		if (iter != m_animation_clip_cache.end())
		{
			result = iter->second;
		}
		else
		{
			result = AnimationLoader::loadAnimationClipData(clip_file_path);
			m_animation_clip_cache.emplace(clip_file_path, result);
		}
		return result;
	}

	std::shared_ptr<AnimationSkeletonMap> AnimationManager::tryLoadAnimationSkeletonMap(std::string skeleton_map_url)
	{
		std::shared_ptr<AnimationSkeletonMap> result;
		auto iter = m_animation_skeleton_map_cache.find(skeleton_map_url);
		if (iter != m_animation_skeleton_map_cache.end())
		{
			result = iter->second;
		}
		else
		{
			result = AnimationLoader::loadAnimationSkeletonMap(skeleton_map_url);
			m_animation_skeleton_map_cache.emplace(skeleton_map_url, result);
		}
		return result;
	}

	std::shared_ptr<BoneBlendMask> AnimationManager::tryLoadSkeletonMask(std::string animation_mask_url)
	{
		std::shared_ptr<BoneBlendMask> result;
		auto iter = m_skeleton_mask_cache.find(animation_mask_url);
		if (iter != m_skeleton_mask_cache.end())
		{
			result = iter->second;
		}
		else
		{
			result = AnimationLoader::loadSkeletonMask(animation_mask_url);
			m_skeleton_mask_cache.emplace(animation_mask_url, result);
			
		}
		return result;
	}

	std::shared_ptr<SkeletonData> AnimationManager::tryLoadSkeletonData(std::string skeleton_data_url)
	{
		std::shared_ptr<SkeletonData> result;
		auto iter = m_skeleton_definition_cache.find(skeleton_data_url);
		if (iter != m_skeleton_definition_cache.end())
		{
			result = iter->second;
		}
		else
		{
			result = AnimationLoader::loadSkeletonData(skeleton_data_url);
			m_skeleton_definition_cache.emplace(skeleton_data_url, result);
		}
		return result;
	}

}