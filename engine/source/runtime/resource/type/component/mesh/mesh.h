#pragma once
#include "core/meta/reflection/reflection.h"
#include "core/math/transform.h"
namespace qyhs
{
	REFLECTION_TYPE(SubMeshRes)
		CLASS(SubMeshRes, Fields)
	{
	public:
		std::string m_material;
		std::string m_obj_file_ref;
		Transform m_transform;
	private:
	};

	REFLECTION_TYPE(MeshComponentRes)
		CLASS(MeshComponentRes, Fields)
	{
	public:
		std::vector<SubMeshRes> m_sub_meshes;
	private:
	};
}