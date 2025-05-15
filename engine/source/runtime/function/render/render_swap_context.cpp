#include "render_swap_context.h"
namespace qyhs
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
			return !(m_swap_data[m_render_swap_data_index].m_game_object_resource.has_value() &&
				     m_swap_data[m_render_swap_data_index].m_camera_swap_data.has_value());

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

	void RenderSwapContext::resetCameraSwapData()
	{
		m_swap_data[m_render_swap_data_index].m_camera_swap_data.reset();
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
