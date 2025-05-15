#include "skeleton.h"
#include <core/math/vector3.h>

namespace qyhs
{
	void Skeleton::applyAnimation(const BlendStateWithClipData& blend_state)
	{
		if (!m_bones)
		{
			return;
		}
		resetSkeleton();
		for (size_t clip_index = 0; clip_index < blend_state.m_animation_clips.size(); ++clip_index)
		{
			const AnimationClip& animation_clip = blend_state.m_animation_clips[clip_index];
			const float phase = blend_state.blend_ratios[clip_index];
			const AnimationSkeletonMap& skeleton_map = blend_state.m_animation_skeleton_maps[clip_index];

			float exact_frame = phase * (animation_clip.total_frame - 1);
			size_t current_frame_low = floor(exact_frame);
			size_t current_frame_height = ceil(exact_frame);
			float lerp_ratio = exact_frame - current_frame_low;
			for (size_t node_index = 0; node_index < animation_clip.node_count && node_index < skeleton_map.convert.size();++node_index)
			{
				AnimationChannel animation_channel = animation_clip.node_channels[node_index];
				size_t bone_index = skeleton_map.convert[node_index];
				float weight = 1;
				if (fabs(weight) < 0.0001f)
				{
					continue;
				}
				if (bone_index == std::numeric_limits<size_t>().max())
				{
					continue;
				}
				Bone* bone = &m_bones[bone_index];
				if (animation_channel.position_keys.size() <= current_frame_height)
				{
					current_frame_height = animation_channel.position_keys.size() - 1;
				}
				if (animation_channel.rotation_keys.size() <= current_frame_height)
				{
					current_frame_height = animation_channel.rotation_keys.size() - 1;
				}
				if (animation_channel.scaling_keys.size() <= current_frame_height)
				{
					current_frame_height = animation_channel.scaling_keys.size() - 1;
				}
				current_frame_low < current_frame_height ? current_frame_low : current_frame_height;
				
				Vector3 position = Vector3::lerp(animation_channel.position_keys[current_frame_low], animation_channel.position_keys[current_frame_height], lerp_ratio);
				Quaternion rotation = Quaternion::nLerp(lerp_ratio, animation_channel.rotation_keys[current_frame_low], animation_channel.rotation_keys[current_frame_height], true);
				Vector3 scale = Vector3::lerp(animation_channel.scaling_keys[current_frame_low], animation_channel.scaling_keys[current_frame_height], lerp_ratio);

				bone->rotate(rotation);
				bone->scale(scale);
				bone->translate(position);
			}
		}
		for (size_t i = 0; i < m_bone_count; i++)
		{
			m_bones[i].update();
		}
	}

	void Skeleton::resetSkeleton()
	{
		for (size_t i = 0; i < m_bone_count; ++i)
		{
			m_bones[i].resetInitialPose();
		}
	}

	void Skeleton::buildSkeleton(const SkeletonData& skeleton_data)
	{
		m_is_flat = skeleton_data.is_flat;
		if (!m_is_flat || !skeleton_data.in_topological_order)
		{
			return;
		}
		if (m_bones != nullptr)
		{
			delete[] m_bones;
		}
		m_bone_count = skeleton_data.bones_map.size();
		m_bones = new Bone[m_bone_count];
		for (size_t i = 0; i < m_bone_count; ++i)
		{
			const RawBone bone_definition = skeleton_data.bones_map[i];
			Bone* parent_bone = find_by_index(m_bones, bone_definition.parent_index, i, m_is_flat);
			m_bones[i].initialize(std::make_shared<RawBone>(bone_definition), parent_bone);
		}
	}

	Bone* Skeleton::find_by_index(Bone* bones, size_t key, size_t size, bool is_flat)
	{
		if (key == std::numeric_limits<int>::max())
			return nullptr;
		if (is_flat)
		{
			if (key >= size)
				return nullptr;
			else
				return &bones[key];
		}
		else
		{
			for (int i = 0; i < size; i++)
			{
				if (bones[i].getID() == key)
					return &bones[i];
			}
		}
		return nullptr;
	}

	AnimationResult Skeleton::outputAnimationResult()
	{
		AnimationResult animation_result;
		for (size_t i = 0; i < m_bone_count; ++i)
		{
			std::shared_ptr<AnimationResultElement> result_element = std::make_shared<AnimationResultElement>();
			Bone* bone = &m_bones[i];
			result_element->index = bone->getID() + 1;
			auto object_matrix = Transform(bone->getDerivedPosition(), bone->getDerivedRotation(), bone->getDerivedScale()).getMatrix();
			auto result_matrix = object_matrix * bone->getInverseTpose();
			result_element->transform = result_matrix.toMatrix4x4_();
			animation_result.node.push_back(*result_element);
		}
		return animation_result;
	}


}