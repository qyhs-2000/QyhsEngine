#pragma once
#include "blend_state.h"
#include "node.h"
#include "animation.h"
namespace QYHS
{
	

	class Skeleton
	{
	public:
		void applyAnimation(const BlendStateWithClipData& blend_state);
		void resetSkeleton();
		void buildSkeleton(const SkeletonData& skeleton_data);
		Bone* find_by_index(Bone* bones, size_t key, size_t size, bool is_flat);
		AnimationResult outputAnimationResult();
	private:
		size_t m_bone_count{ 0 };
		Bone* m_bones{ nullptr };
		bool m_is_flat{ false };
	};
}