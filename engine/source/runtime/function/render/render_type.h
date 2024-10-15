#pragma once

#include <cstddef>
#include <functional>
#include <cstdint>
#include <memory>
#include <string>
#include <resource/type/global_rendering_resource.h>
namespace QYHS
{
	enum PIXEL_FORMAT
	{
		FORMAT_UNKNOWN = 0,
        FORMAT_R8G8B8_UNORM,
        FORMAT_R8G8B8_SRGB,
        FORMAT_R8G8B8A8_UNORM,
        FORMAT_R8G8B8A8_SRGB,
        FORMAT_R32G32_FLOAT,
        FORMAT_R32G32B32_FLOAT,
        FORMAT_R32G32B32A32_FLOAT
	};

	struct LevelIBLResource
	{
		SkyBoxSpecularMap m_skybox_specular_map;
	};

	struct LevelResourceDesc
	{
		LevelIBLResource m_ibl_resource_desc;
	};

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
		float nx, ny, nz;   //normal 
		float tx, ty, tz;   //tangent
		float u, v;			//uv 
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

		PIXEL_FORMAT m_format{PIXEL_FORMAT::FORMAT_UNKNOWN};
		void* m_pixels{ nullptr };
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