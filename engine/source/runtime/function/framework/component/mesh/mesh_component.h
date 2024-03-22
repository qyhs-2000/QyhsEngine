#pragma once
#include "../component.h"
#include "function/global/global_context.h"
namespace QYHS
{
	REFLECTION_TYPE(MeshComponent)
	CLASS(MeshComponent :public Component,WhiteListFields)
	{
	public:
		virtual void tick(double time) override;
	private:
		bool is_dirty = true;
	};
}