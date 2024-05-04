#pragma once

#include <cstddef>
#include <functional>
#include <cstdint>
#include <memory>
#include <string>
namespace QYHS
{
	struct MeshSourceDesc
	{
		std::string mesh_source_file;
		bool   operator==(const MeshSourceDesc& rhs) const { return mesh_source_file == rhs.mesh_source_file; }
		size_t getHashValue() const { return std::hash<std::string> {}(mesh_source_file); }
	};

	struct MaterialSourceDesc
	{
		std::string m_base_color_file;
		bool   operator==(const MaterialSourceDesc& rhs) const { return m_base_color_file == rhs.m_base_color_file; }
		size_t getHashValue() const { return std::hash<std::string> {}(m_base_color_file); }
	};

	struct BufferData
	{
		void* m_data{ nullptr };
		size_t m_size{ 0 };

		BufferData(size_t size)
		{
			m_size = size;
			m_data = malloc(size);
		}
		~BufferData()
		{
			if (m_data)
			{
				free(m_data);
			}
		}
	};

	struct MeshVertexDataDefinition
	{
		float x, y, z;
		float u, v;
	};

	struct StaticMeshData
	{
		std::shared_ptr<BufferData>	m_vertex_buffer;
		std::shared_ptr<BufferData>	m_index_buffer;
	};

	struct RenderMeshData
	{
		StaticMeshData		m_static_mesh_data;
	};

	struct TextureData
	{
		uint32_t m_width;
		uint32_t m_height;

		void* data;
	};

	struct RenderMaterialData
	{
		std::shared_ptr<TextureData> m_base_color_texture;
	};
}

template<>
struct std::hash<QYHS::MeshSourceDesc>
{
	size_t operator()(const QYHS::MeshSourceDesc& rhs) const noexcept { return rhs.getHashValue(); }
};
template<>
struct std::hash<QYHS::MaterialSourceDesc>
{
	size_t operator()(const QYHS::MaterialSourceDesc& rhs) const noexcept { return rhs.getHashValue(); }
};