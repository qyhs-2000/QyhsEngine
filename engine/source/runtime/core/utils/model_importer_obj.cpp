#include "model_importer_obj.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace QYHS
{
	StaticMeshData loadStaticMesh(const std::string& mesh_file)
	{
		StaticMeshData mesh_data;
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		std::vector<MeshVertexDataDefinition> mesh_vertices;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, mesh_file.c_str())) {
			throw std::runtime_error(warn + err);
		}
		for (auto& shape : shapes)
		{
			size_t index_offset = 0;
			for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f)
			{
				bool with_texcoord = true;
				bool with_normal = true;
				size_t fv = shape.mesh.num_face_vertices[f];
				if (fv != 3)
				{
					continue;
				}

				Vector3 vertex[3];
				Vector2 uv[3];

				for (size_t v = 0; v < fv; ++v)
				{
					auto idx = shape.mesh.indices[index_offset + v];
					vertex[v].x = static_cast<float>(attrib.vertices[3 * size_t(idx.vertex_index) + 0]);
					vertex[v].y = static_cast<float>(attrib.vertices[3 * size_t(idx.vertex_index) + 1]);
					vertex[v].z = static_cast<float>(attrib.vertices[3 * size_t(idx.vertex_index) + 2]);

					if (idx.texcoord_index >= 0)
					{
						uv[v].x = static_cast<float>(attrib.texcoords[2 * size_t(idx.texcoord_index) + 0]);
						uv[v].y = static_cast<float>(attrib.texcoords[2 * size_t(idx.texcoord_index) + 1]);
					}
					else
					{
						with_texcoord = false;
					}


				}
				index_offset += fv;
				if (!with_texcoord)
				{
					uv[0] = Vector2(0.5f, 0.5f);
					uv[1] = Vector2(0.5f, 0.5f);
					uv[2] = Vector2(0.5f, 0.5f);
				}

				for (size_t i = 0; i < 3; ++i)
				{
					MeshVertexDataDefinition mesh_vert{};
					mesh_vert.x = vertex[i].x;
					mesh_vert.y = vertex[i].y;
					mesh_vert.z = vertex[i].z;

					mesh_vert.u = uv[i].x;
					mesh_vert.v = uv[i].y;

					mesh_vertices.push_back(mesh_vert);
				}
			}
		}
		uint32_t stride = sizeof(MeshVertexDataDefinition);
		mesh_data.m_vertex_buffer = std::make_shared<BufferData>(mesh_vertices.size() * stride);
		mesh_data.m_index_buffer = std::make_shared<BufferData>(mesh_vertices.size() * sizeof(uint16_t));

		assert(mesh_vertices.size() < std::numeric_limits<uint16_t>::max());
		uint16_t* indices = (uint16_t*)mesh_data.m_index_buffer->m_data;
		for (size_t i = 0; i < mesh_vertices.size(); ++i)
		{
			((MeshVertexDataDefinition*)(mesh_data.m_vertex_buffer->m_data))[i] = mesh_vertices[i];
			indices[i] = static_cast<uint16_t>(i);
		}
		return mesh_data;
	}
}