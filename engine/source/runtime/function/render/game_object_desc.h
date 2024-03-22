#pragma once
#include <vector>
#include "function/framework/game_object/game_object_id_allocator.h"
namespace QYHS
{

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

	private:
		GameObjectPartId m_part_id;
	};

	class GameObjectDesc
	{
	public:
		GameObjectDesc() :m_go_id(0) {};
		GameObjectDesc(size_t go_id, const std::vector<GameObjectPartDesc>& parts) :m_go_id(go_id), m_object_parts(parts) {}
		int getObjectPartSize();
		std::vector<GameObjectPartDesc>& getGameObjectParts();
		GameObjectID id;
	private:
		std::vector<GameObjectPartDesc>  m_object_parts;
		GameObjectID m_go_id;
	};


}

template<>
struct std::hash<QYHS::GameObjectPartId>
{
	size_t operator()(const QYHS::GameObjectPartId& rhs) const noexcept { return rhs.getHashValue(); }
};


