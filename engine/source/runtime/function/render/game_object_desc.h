#pragma once
#include <vector>
#include "function/framework/game_object/game_object_id_allocator.h"
#include "core/math/matrix4.h"
namespace QYHS
{
	class GameObjectMeshDesc
	{
	public:
		std::string m_mesh_file;
	private:
	};

	class GameObjectMaterialDesc
	{
	public:
		std::string m_base_colour_texture_file;
		bool m_with_texture{ false };
	private:
	};

	class GameObjectTransformDesc
	{
	public:
		Matrix4x4 m_transform_matrix;
	private:
	};


	struct GameObjectPartId
	{
		GameObjectID	m_go_id;
		size_t		m_part_id;
		size_t getHashValue() const { return m_go_id ^ (m_part_id << 1); }
		bool   operator==(const GameObjectPartId& rhs) const { return m_go_id == rhs.m_go_id && m_part_id == rhs.m_part_id; }
	};

	class GameObjectPartDesc
	{
	public:
		GameObjectMeshDesc  m_mesh_desc;
		GameObjectMaterialDesc m_material_desc;
		GameObjectTransformDesc m_transform_desc;
	private:
		GameObjectPartId m_part_id;
	};

	class GameObjectDesc
	{
	public:
		GameObjectDesc() :m_go_id(0) {};
		GameObjectDesc(size_t go_id, const std::vector<GameObjectPartDesc>& parts) :m_go_id(go_id), m_object_parts(parts) {}
		int getObjectPartSize();
		GameObjectID getId() { return m_go_id; }
		std::vector<GameObjectPartDesc>& getGameObjectParts();
		GameObjectID m_go_id;
	private:
		std::vector<GameObjectPartDesc>  m_object_parts;
	};


}

template<>
struct std::hash<QYHS::GameObjectPartId>
{
	size_t operator()(const QYHS::GameObjectPartId& rhs) const noexcept { return rhs.getHashValue(); }
};


