#pragma once
#include "../component.h"
#include "function/global/global_context.h"
#include "resource/type/component/mesh/mesh.h"
#include <function/render/game_object_desc.h>
namespace QYHS
{
	class MeshComponentRes;
	REFLECTION_TYPE(MeshComponent)
	CLASS(MeshComponent :public Component,WhiteListFields)
	{
		REFLECTION_BODY(MeshComponent)
	public:
		virtual void tick(double time) override;
		virtual void postLoadResource(std::weak_ptr<GameObject> parent_object) override;
		float test2;
	private:
		std::vector<GameObjectPartDesc> m_raw_meshes;
		META(Enable)
		MeshComponentRes m_mesh_res;
		bool is_dirty = true;
		float test;
	};
}