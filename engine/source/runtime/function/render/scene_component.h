#pragma once
#include "function/render/rhi/rhi.h"
#include <shader/shader_interop_renderer.h>
#include "function/framework/component/ecs.h"
#include "function/render/primitive.h"
#include "core/math/math_library.h"
#include "resource/resource_manager.h"
namespace qyhs::scene
{
	class Component
	{
	public:
		virtual void serialize(Archive& archive, ecs::EntitySerializer& seri) = 0;
	private:
	};
	class ObjectComponent :public Component
	{
	public:
		ecs::Entity mesh_entity = ecs::INVALID_ENTITY;
		uint32_t mesh_index;
		uint32_t lod = 0;
		XMFLOAT3 center = XMFLOAT3(0, 0, 0);
		float radius = 0;
		uint32_t filter_mask = 0;
		uint32_t filter_mask_dynamic = 0;
		inline uint32_t getFilterMask()const
		{
			return filter_mask | filter_mask_dynamic;
		}

		virtual void serialize(Archive& archive, ecs::EntitySerializer& seri) override;
	};

	class HierarchyComponent:public Component
	{
	public:
		qyhs::ecs::Entity parent_id;
		virtual void serialize(Archive& archive, ecs::EntitySerializer& seri) override;

	private:
	};

	class MaterialComponent :public Component
	{
	public:
		enum FLAGS
		{
			EMPTY = 0,
			DOUBLE_SIDE = 1 << 0,
			DIRTY = 1 << 1,
			PREFER_UNCOMPRESSED_TEXTURES = 1 << 2,
			DISABLE_TEXTURE_STREAMING = 1 << 3
		};
		enum
		{
			BASECOLOR_MAP,
			TEXTUREMAP_COUNT
		};

		enum SHADERTYPE
		{
			SHADERTYPE_PBR,
			SHADERTYPE_COUNT
		}shader_type = SHADERTYPE_PBR;

		enum TEXTURESLOT
		{
			BASECOLORMAP,

			TEXTURESLOT_COUNT
		};
		XMFLOAT4 base_color = XMFLOAT4(1, 1, 1, 1);
		struct TextureMap
		{
			std::string name;
			uint32_t uvset = 0;
			resourcemanager::Resource resource;
			const graphics::GPUResource* getGPUResource()const
			{
				if (!resource.isValid() || !resource.getTexture().isValid())
				{
					return nullptr;
				}
				return &resource.getTexture();
			}
		} textures[TEXTUREMAP_COUNT];
		inline enums::BLENDMODE getBlendMode() const { return userBlendMode; }
		inline bool isDoubleSided()const { return _flags & DOUBLE_SIDE; }
		inline bool isPreferUnCompressedTexturesEnabled()const { return _flags & PREFER_UNCOMPRESSED_TEXTURES; }
		inline bool isTextureStreamingDisabled()const { return _flags & DISABLE_TEXTURE_STREAMING; }
		resourcemanager::Flags getTextureSlotResourceFlags(TEXTURESLOT slot);
		uint32_t getFilterMask()const;
		int sampler_descriptor = -1;
		BLENDMODE userBlendMode = BLENDMODE::BLENDMODE_OPAQUE;
		uint32_t _flags = 0;
		inline static const std::vector<std::string> shadertype_definitions[] = {
			{} // SHADERTYPE_PBR,
		};

		void writeShaderMaterial(ShaderMaterial* dst);
		void createRenderData();

		virtual void serialize(Archive& archive, ecs::EntitySerializer& seri) override;
	private:
	};

	class NameComponent :public Component
	{
	public:
		inline void operator=(const std::string  str) { name = str; }
		virtual void serialize(Archive& archive, ecs::EntitySerializer& seri) override;
	private:
		std::string name;
	};

	class TransformComponent :public Component
	{
	public:
		enum
		{
			NOT_DIRTY = 0,
			DIRTY = 1 << 0
		};
		void applyWorldToLocal();
		void MatrixTransform(XMMATRIX matrix);
		void updateTransform();
		void updateTransformParented(TransformComponent* parent_transform);
		XMMATRIX getLocalMatrix()const;
		bool isDirty()const { return _flag & DIRTY; }
		void translate(const XMFLOAT3& move);
		void rotateRollPitchYaw(const XMFLOAT3& value);
		void setDirty(bool value = true)
		{
			if (value)
			{
				_flag |= DIRTY;
			}
			else
			{
				_flag &= DIRTY;
			}
		}
		XMFLOAT4X4 world = math::getIdentityMatrix();
		XMFLOAT3 local_scale = XMFLOAT3(1, 1, 1);
		XMFLOAT3 local_position = XMFLOAT3(0, 0, 0);
		XMFLOAT4 local_rotation = XMFLOAT4(0, 0, 0, 1);

		virtual void serialize(Archive& archive, ecs::EntitySerializer& seri) override;
	private:
		uint32_t _flag = DIRTY;
	};

	class MeshComponent :public Component
	{
	public:
		enum FLAGS
		{
			EMPTY = 0,
			RENDERABLE = 1 << 0,
			DOUBLE_SIDE = 1 << 1
		};
		struct MeshSubset
		{
			ecs::Entity materialID = ecs::INVALID_ENTITY;
			uint32_t index_count = 0;
			uint32_t index_offset = 0;

			uint32_t material_index = 0;

		};
		struct Vertex_POS16
		{
			uint16_t x = 0;
			uint16_t y = 0;
			uint16_t z = 0;
			uint16_t w = 0;

			void fromFull(const primitive::AABB& aabb, XMFLOAT3  pos, uint8_t wind)
			{
				pos = math::inverseLerp(aabb._min, aabb._max, pos);
				x = uint16_t(pos.x * 65535.0f);
				y = uint16_t(pos.y * 65535.0f);
				z = uint16_t(pos.z * 65535.0f);
				w = uint16_t((float(wind) / 255.0f) * 65535.0f);
			}
			static constexpr graphics::Format FORMAT = graphics::Format::R16G16B16A16_UNORM;
		};
		struct Vertex_POS32
		{
			float x = 0;
			float y = 0;
			float z = 0;

			constexpr void fromFull(const XMFLOAT3& pos)
			{
				x = pos.x;
				y = pos.y;
				z = pos.z;
			}
			inline XMVECTOR LoadPOS() const
			{
				return XMVectorSet(x, y, z, 1);
			}
			constexpr XMFLOAT3 GetPOS() const
			{
				return XMFLOAT3(x, y, z);
			}
			static constexpr graphics::Format FORMAT = graphics::Format::R32G32B32_FLOAT;
		};
		struct Vertex_Color
		{
			uint32_t color = 0;
			static constexpr graphics::Format FORMAT = graphics::Format::R8G8B8A8_UNORM;
		};

		struct BufferView
		{
			uint64_t offset = ~0ull;
			uint64_t size = 0ull;
			int subresource_srv = -1;
			int descriptor_srv = -1;
			int subresource_uav = -1;
			int descriptor_uav = -1;

			constexpr bool IsValid() const
			{
				return offset != ~0ull;
			}
		};

		struct Vertex_TEX
		{
			uint16_t x = 0;
			uint16_t y = 0;

			constexpr void FromFULL(const XMFLOAT2& uv, const XMFLOAT2& uv_range_min = XMFLOAT2(0, 0), const XMFLOAT2& uv_range_max = XMFLOAT2(1, 1))
			{
				x = uint16_t(math::inverseLerp(uv_range_min.x, uv_range_max.x, uv.x) * 65535.0f);
				y = uint16_t(math::inverseLerp(uv_range_min.y, uv_range_max.y, uv.y) * 65535.0f);
			}
			static constexpr graphics::Format FORMAT = graphics::Format::R16G16_UNORM;
		};
		struct Vertex_UVS
		{
			Vertex_TEX uv0;
			Vertex_TEX uv1;
			static constexpr graphics::Format FORMAT = graphics::Format::R16G16B16A16_UNORM;
		};
		XMFLOAT2 uv_range_min = XMFLOAT2(0, 0);
		XMFLOAT2 uv_range_max = XMFLOAT2(1, 1);
		Format position_format = Vertex_POS16::FORMAT;
		uint32_t geometry_offset;
		std::vector<MeshSubset> subsets;
		graphics::GPUBuffer general_buffer;   //index buffer and all static vertex buffer
		primitive::AABB aabb;
		
		inline uint32_t getLodCount() const { return subsets_per_lod == 0 ? 1 : (uint32_t)(subsets.size() / subsets_per_lod); }
		void createRenderData();
		void deleteRenderData();
		inline graphics::IndexBufferFormat getIndexFormat() const { return graphics::getIndexBufferFormat((uint32_t)vertex_positions.size()); }
		inline size_t getIndexBufferStride() const { return getIndexFormat() == graphics::IndexBufferFormat::UINT32 ? sizeof(uint32_t) : sizeof(uint16_t); }
		void getLodSubsetRange(uint32_t lod, int& first_subset, int& last_subset) const
		{
			first_subset = 0;
			last_subset = subsets.size();
			if (subsets_per_lod > 0)
			{
				lod = std::min(lod, getLodCount() - 1);
				first_subset = lod * subsets_per_lod;
				last_subset = first_subset + subsets_per_lod;
			}
		}
		void setDoubleSided(bool value) {
			if (value)
			{
				_flags |= DOUBLE_SIDE;
			}
			else
			{
				_flags &= (uint32_t)(~DOUBLE_SIDE);
			}
		}
		inline bool isDoubleSided()const { return _flags & DOUBLE_SIDE; }
		uint32_t subsets_per_lod = 0;
		std::vector<uint32_t> indices;
		std::vector<XMFLOAT3> vertex_positions;
		std::vector<XMFLOAT3> vertex_normals;
		std::vector<XMFLOAT4> vertex_tangents;
		std::vector<XMFLOAT2> vertex_uvset_0;
		std::vector<XMFLOAT2> vertex_uvset_1;
		std::vector<XMUINT4> vertex_boneindices;
		std::vector<XMFLOAT4> vertex_boneweights;
		std::vector<XMUINT4> vertex_boneindices2;
		std::vector<XMFLOAT4> vertex_boneweights2;
		std::vector<XMFLOAT2> vertex_atlas;
		std::vector<uint32_t> vertex_colors;
		std::vector<uint8_t> vertex_windweights;
		uint32_t _flags = RENDERABLE;

		BufferView ib;
		BufferView vb_pos_wind;
		BufferView vb_uvs;

		virtual void serialize(Archive& archive, ecs::EntitySerializer& seri) override;
	private:
	};

	class CameraComponent :public Component
	{
	public:
		enum FLAGS
		{
			EMPTY = 0,
			DIRTY = 1 << 0,
			CUSTOM_PROJECTION = 1 << 1,
			ORTHO = 1 << 2,
		};
		float fov = 60 * (XM_PI / 180);     //60 degree is suitable for human eye
		float width = 0.0f;
		float height = 0.0f;
		uint shadercamera_options = SHADERCAMERA_OPTION_NONE;
		bool isOrtho()const { return _flags & ORTHO; }
		XMMATRIX getViewProjection() const { return XMLoadFloat4x4(&view_proj); }
		XMMATRIX getInvViewProjection() const { return XMLoadFloat4x4(&inv_view_projection); }
		XMMATRIX getView() const { return XMLoadFloat4x4(&view); }
		XMMATRIX getProjection() const { return XMLoadFloat4x4(&proj); }
		XMMATRIX getInvView() const { return XMLoadFloat4x4(&inv_view); }
		XMMATRIX getInvProjection() const { return XMLoadFloat4x4(&inv_projection); }
		void transformCamera(const XMMATRIX& transform_matrix);
		void transformCamera(const TransformComponent& transform) { transformCamera(XMLoadFloat4x4(&transform.world)); }
		void updateCamera();
		inline void SetDirty(bool value = true) { if (value) { _flags |= DIRTY; } else { _flags &= ~DIRTY; } };
		XMFLOAT4X4 view_proj;
		XMFLOAT4X4 view;
		XMFLOAT4X4 proj;
		XMFLOAT4X4 inv_view, inv_projection, inv_view_projection;
		XMFLOAT3X3 rotation_matrix;
		XMFLOAT3 look_at = XMFLOAT3(0, 0, 1);
		XMFLOAT3 up = XMFLOAT3(0, 1, 0);
		XMFLOAT3 eye = XMFLOAT3(0, 0, 0);
		float z_near = 0.1f;
		float z_far = 5000.0f;
		primitive::Frustum frustum;

		virtual void serialize(Archive& archive, ecs::EntitySerializer& seri) override;
	private:
		uint32_t _flags = 0;
	};

	CameraComponent& getCamera();
}