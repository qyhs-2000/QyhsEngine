#include "animation_component.h"
#include "animation_manager.h"
namespace qyhs
{
	void AnimationComponent::postLoadResource(std::weak_ptr<GameObject> parent_object)
	{
		Component::postLoadResource(parent_object);
		std::shared_ptr<SkeletonData> skeleton_data = AnimationManager::tryLoadSkeletonData(m_animation_res.skeleton_file_path);
		m_skeleton.buildSkeleton(*skeleton_data);
	}

	void AnimationComponent::tick(double delta_time)
	{
		m_animation_res.blend_state.blend_ratio[0] += (delta_time / m_animation_res.blend_state.blend_clip_file_length[0]);
		m_animation_res.blend_state.blend_ratio[0] -= floor(m_animation_res.blend_state.blend_ratio[0]);
		m_skeleton.applyAnimation(AnimationManager::getBlendStateWithClipData(m_animation_res.blend_state));
		m_animation_res.m_animation_result = m_skeleton.outputAnimationResult();
	}

	
}