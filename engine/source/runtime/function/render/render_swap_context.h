#pragma once
#include <optional>
#include <deque>
#include "game_object_desc.h"
#include "function/render/render_camera.h"
namespace qyhs
{
	struct GameObjectResource
	{
		std::deque<GameObjectDesc>  game_objects;
		bool isEmpty();
		GameObjectDesc& getGameObject();
		void pop();
	};

	struct CameraSwapData
	{
		std::optional<float > m_fov_x;
		std::optional<Matrix4x4 >m_view_matrix;
		std::optional<RenderCameraType> m_camera_type;
	};

	struct SwapData
	{
		std::optional<GameObjectResource>	m_game_object_resource;
		std::optional<CameraSwapData>       m_camera_swap_data;
		void addDirtyGameObject(GameObjectDesc && parts)
		{
			if (!m_game_object_resource.has_value())
			{
				GameObjectResource game_object_res;
				m_game_object_resource = game_object_res;
			}
			m_game_object_resource.value().game_objects.push_back(parts);
		}
	};

	enum SwapDataType :uint8_t
	{
		LogicSwapDataType = 0,
		RenderSwapDataType,
		SwapDataTypeCount
	};

	class RenderSwapContext
	{
	public:
		void swapLogicRenderData();
		bool isReadyToSwap();
		void swap();
		SwapData& getRenderSwapData() { return m_swap_data[m_render_swap_data_index]; }
		SwapData& getLogicSwapData() { return m_swap_data[m_logic_swap_data_index]; }
		void resetGameObjectRenderSwapData();
		void resetCameraSwapData();
	private:
		uint8_t				m_render_swap_data_index{ RenderSwapDataType };
		uint8_t				m_logic_swap_data_index{ LogicSwapDataType };
		SwapData			m_swap_data[SwapDataTypeCount];
	};
}