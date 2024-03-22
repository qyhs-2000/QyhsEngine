#include "mesh_component.h"
//#include <function/render/game_object_desc.h>
#include <function/global/global_context.h>
#include <function/render/render_swap_context.h>
#include <function/render/render_system.h>
void QYHS::MeshComponent::tick(double time)
{
	//dirty means that something about the component has changed,so component must be reload
	if (is_dirty)
	{

		RenderSwapContext &swap_context = g_runtime_global_context.m_render_system->getSwapContext();
		SwapData& logic_swap_data = swap_context.getLogicSwapData();

		std::vector<GameObjectPartDesc>	dirty_mesh_parts;

		logic_swap_data.addDirtyGameObject(GameObjectDesc{ parent_object.lock()->getObjectId(),dirty_mesh_parts });
	}

	is_dirty = false;
	
}
