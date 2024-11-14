#pragma once
#include "core/math/math.h"
#include "core/math/vector3.h"
#include "function/framework/game_object/game_object.h"
namespace QYHS
{
	class Character
	{
	public:
		Character() {};
		Character(std::shared_ptr<GameObject> gobject);
		const Vector3 getPosition() { return m_position; }
		const Quaternion getRotation() { return m_rotation; }
		void setRotation(Quaternion rotation) { m_rotation = rotation; }
		void setGameObject(std::shared_ptr<GameObject> gobject);
		GameObjectID getObjectID() { return m_character_object->getObjectId(); }
	private:
		Vector3 m_position;
		Quaternion m_rotation;
		std::shared_ptr<GameObject> m_character_object;
	};
}