#pragma once
#include "core/math/quat.h"
#include "core/math/vector3.h"
#include "core/math/matrix4.h"
#include "resource/type/data/skeleton_data.h"
namespace qyhs
{
	class Node
	{
	public:
		enum class TransformSpace
		{
			LOCAL,
			AREN
		};
		void rotate(const Quaternion& rotation, TransformSpace relative_to = TransformSpace::LOCAL);
		void translate(const Vector3& translation, TransformSpace relative_to = TransformSpace::AREN);
		void scale(const Vector3& scale);
		void update();
		void updateDerivedTransform();
		inline void setDirty() { m_dirty = true; }
		inline Quaternion getDerivedRotation() const { return m_derived_rotation; }
		inline Vector3 getDerivedPosition()const { return m_derived_position; }
		inline Vector3 getDerivedScale()const { return m_derived_scale; }
		Matrix4x4 getInverseTpose()const { return m_inverse_Tpose; }
	protected:
		std::string m_name;
		Quaternion  m_rotation;
		Vector3	    m_position;
		Vector3	    m_scale;
		
		Quaternion  m_derived_rotation;
		Vector3	    m_derived_position;
		Vector3	    m_derived_scale;

		Vector3     m_initial_position;
		Vector3     m_initial_scale;
		Quaternion  m_initial_rotation;
		bool m_dirty{ true };
		Node* m_parent{ nullptr };
		Matrix4x4 m_inverse_Tpose;
	};

	class Bone :public Node
	{
	public:
		void initialize(std::shared_ptr<RawBone> definition, Bone* parent_bone);
		void resetInitialPose();
		void setAsInitialPose();
		void setRotation(const Quaternion &orientation);
		inline void setPosition(const Vector3& position);
		inline void setScale(const Vector3& scale);
		size_t getID()const;
		
	private:
		
		
		std::shared_ptr<RawBone> m_definition;
		
		
	};
}