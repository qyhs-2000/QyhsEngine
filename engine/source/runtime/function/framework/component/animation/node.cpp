#include "node.h"

namespace qyhs
{
	void Bone::initialize(std::shared_ptr<RawBone> definition, Bone* parent_bone)
	{
		m_definition = definition;
		if (definition)
		{
			m_name = definition->name;
			setRotation(definition->binding_pose.m_rotation);
			setPosition(definition->binding_pose.m_position);
			setScale(definition->binding_pose.m_scale);
			m_inverse_Tpose = definition->tpose_matrix;
			setAsInitialPose();
		}
		m_parent = parent_bone;
	}

	void Bone::resetInitialPose()
	{
		m_position = m_initial_position;
		m_rotation = m_initial_rotation;
		m_scale = m_initial_scale;
		setDirty();
	}
	void Bone::setAsInitialPose()
	{
		m_initial_position = m_position;
		m_initial_rotation = m_rotation;
		m_initial_scale = m_scale;

	}
	void Bone::setRotation(const Quaternion &orientation)
	{
		if (orientation.isNaN())
		{
			m_rotation = Quaternion::IDENTITY;
		}
		else
		{
			m_rotation = orientation;
			m_rotation.normalise();
		}
		setDirty();
	}
	
	inline void Bone::setPosition(const Vector3& position)
	{
		m_position = position; 
		setDirty();
	}
	
	inline void Bone::setScale(const Vector3& scale)
	{
		m_scale = scale;
		setDirty();
	}

	size_t Bone::getID() const
	{
		if (m_definition)
		{
			return m_definition->index;
		}
		return std::numeric_limits<size_t>::max();
	}
	
	void Node::rotate(const Quaternion& rotation, TransformSpace relative_to)
	{
		Quaternion q = rotation;
		q.normalise();
		switch (relative_to)
		{
		case qyhs::Node::TransformSpace::LOCAL:
			m_rotation = m_rotation * q;
			break;
		case qyhs::Node::TransformSpace::AREN:
			m_rotation = q * m_rotation;
			break;
		default:
			break;
		}
		setDirty();
	}

	void Node::translate(const Vector3& translation, TransformSpace relative_to)
	{
		switch (relative_to)
		{
		case qyhs::Node::TransformSpace::LOCAL:
			m_position = m_position + m_rotation*translation;
			break;
		case qyhs::Node::TransformSpace::AREN:
			m_position = m_position + translation;
			break;
		default:
			break;
		}
		setDirty();
	}

	void Node::scale(const Vector3& scale)
	{
		m_scale = m_scale * scale;
		setDirty();
	}

	void Node::update()
	{
		updateDerivedTransform();
		m_dirty = false;
	}

	void Node::updateDerivedTransform()
	{
		if (m_parent)
		{
			const Quaternion& parent_rotation = m_parent->getDerivedRotation();
			m_derived_rotation = parent_rotation * m_rotation;
			m_derived_rotation.normalise();

			const Vector3& parent_scale = m_parent->getDerivedScale();
			m_derived_scale = parent_scale * m_scale;

			m_derived_position = parent_rotation * (parent_scale * m_position);
			m_derived_position = m_derived_position + m_parent->getDerivedPosition();
		}
		else
		{
			m_derived_position = m_position;
			m_derived_rotation = m_rotation;
			m_derived_scale = m_scale;
		}
	}

}