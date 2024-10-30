#include "render_swap_context.h"
namespace QYHS
{

	void RenderSwapContext::swapLogicRenderData()
	{
		if (isReadyToSwap())
		{
			swap();
		}
	}

	bool RenderSwapContext::isReadyToSwap()
	{
		{
			return !(m_swap_data[m_render_swap_data_index].m_game_object_resource.has_value());

		}
	}

	void RenderSwapContext::swap()
	{

		std::swap(m_render_swap_data_index, m_logic_swap_data_index);
	}

	void RenderSwapContext::resetGameObjectRenderSwapData()
	{
		m_swap_data[m_render_swap_data_index].m_game_object_resource.reset();
	}

	bool GameObjectResource::isEmpty()
	{
		return game_objects.empty();
	}

	GameObjectDesc& GameObjectResource::getGameObject()
	{
		return game_objects.front();
	}

	void GameObjectResource::pop()
	{
		game_objects.pop_front();
	}
}
