#include "transform_component.h"
#include <iostream>
namespace QYHS
{
	void TransformComponent::postLoadResource(std::weak_ptr<GameObject> parent_object)
	{
		Component::postLoadResource(parent_object);
		if (parent_object.lock()->getParent() == nullptr)
		{
			updateWorldMatrix();
		}
		m_is_dirty = true;

	}

	void TransformComponent::setPosition(Vector3 position)
	{
		m_transform.m_position = position; 
		m_is_dirty = true; 
	}

	void TransformComponent::setScale(Vector3 scale)
	{
		m_transform.m_scale = scale;
		m_is_dirty = true;
	}

	//set transform from world space to local space
	void TransformComponent::matrixTransform(const Matrix4x4& parent_inverse_matrix)
	{
		Vector3 position;
		Quaternion rotation;
		Vector3 scale;

		(m_transform.getMatrix()* parent_inverse_matrix).decomposition(m_transform.m_position, m_transform.m_scale, m_transform.m_rotation);

	}

	//update world matrix by local transform
	void TransformComponent::updateWorldMatrix()
	{
		world_matrix = m_transform.getMatrix();
	}

	//update world matrix by parent transform
	void TransformComponent::updateWorldMatrix_Parented(const TransformComponent& parent_transform_comp)
	{
		Matrix4x4 local_matrix = m_transform.getMatrix();
		Matrix4x4 parent_world = parent_transform_comp.world_matrix;
		this->world_matrix = local_matrix * parent_world;
	}

	//make transform reference to world matrix
	void TransformComponent::applyTransform()
	{
		world_matrix.decomposition(m_transform.m_position, m_transform.m_scale, m_transform.m_rotation);
		setDirty(true);
	}
}