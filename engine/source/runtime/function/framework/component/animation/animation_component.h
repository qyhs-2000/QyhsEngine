#pragma once

#include "function/framework/component/component.h"
#include "animation.h"
#include "blend_state.h"
#include "skeleton.h"
namespace QYHS
{

	REFLECTION_TYPE(AnimationComponent)
		CLASS(AnimationComponent:public Component, WhiteListFields)
	{
		REFLECTION_BODY(AnimationComponent)
	public:
		virtual void postLoadResource(std::weak_ptr<GameObject> parent_object) override;
		void tick(double delta_time) override;
		AnimationResult getResult()const { return m_animation_res.m_animation_result; }
	protected:
		META(Enable)
		AnimationComponentRes m_animation_res;
		Skeleton m_skeleton;
	};
}