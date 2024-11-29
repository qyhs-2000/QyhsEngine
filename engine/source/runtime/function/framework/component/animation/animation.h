#pragma once
#include <vector>
#include "core/math/matrix4.h"
namespace QYHS
{
	class Matrix4x4;
	class AnimationResultElement
	{
	public:
		size_t index;
		Matrix4x4 transform;
	};

	class AnimationResult
	{
	public:
		std::vector<AnimationResultElement> node;
	};

	REFLECTION_TYPE(BlendState)
		CLASS(BlendState, Fields)
	{
	public:
		std::vector<std::string >blend_anim_skel_map_path;
		std::vector<std::string> blend_clip_file_path;
		std::vector<std::string> blend_mask_file_path;
		std::vector<float> blend_clip_file_length;
		std::vector<float> blend_ratio;
		std::vector<float> blend_weight;
		int clip_count;
	private:
	};

	REFLECTION_TYPE(AnimationComponentRes)
		CLASS(AnimationComponentRes, Fields)
	{
		REFLECTION_BODY(AnimationComponentRes)
	public:
		std::string skeleton_file_path;
		float frame_position;
		BlendState blend_state;
		META(Disable)
			AnimationResult m_animation_result;

	private:

	};
}