#pragma once
#include <vector>
#include "function/framework/game_object/game_object_id_allocator.h"
#include "core/math/matrix4.h"
namespace qyhs
{
	typedef size_t Mesh_ID;
	typedef size_t Material_ID;
	
	class SkeletonAnimationResultTransform
	{
	public:
		Matrix4x4 m_matrix;
	};

	class SkeletonBindingDesc
	{
	public:
		std::string m_skeleton_binding_file;
	};

	class SkeletonAnimationResult
	{
	public:
		std::vector<SkeletonAnimationResultTransform> m_transforms;
	};

	class GameObjectMeshDesc
	{
	public:
		std::string m_mesh_file;
		Mesh_ID     mesh_asset_id;
	private:
	};

	class GameObjectMaterialDesc
	{
	public:
		std::string m_base_colour_texture_file;
		Material_ID m_base_color_texture_asset_id;
		bool m_with_texture{ false };
	private:
	};

	class GameObjectTransformDesc
	{
	public:
		Matrix4x4 m_transform_matrix{Matrix4x4::IDENTITY};
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
		Mesh_ID  m_mesh_id;
		Material_ID m_material_id;
		GameObjectTransformDesc m_transform_desc;
		SkeletonAnimationResult m_skeleton_animation_result;
		SkeletonBindingDesc m_skeleton_binding_desc;
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
struct std::hash<qyhs::GameObjectPartId>
{
	size_t operator()(const qyhs::GameObjectPartId& rhs) const noexcept { return rhs.getHashValue(); }
};


