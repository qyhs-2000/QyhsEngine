#include "scene_component.h"
#include "core/math/math_library.h"
#include "function/render/renderer.h"
#include <iostream>

namespace qyhs::scene
{
	CameraComponent& getCamera()
	{
		static CameraComponent camera;
		return camera;
	}

	void TransformComponent::applyWorldToLocal()
	{
		setDirty();
		XMVECTOR s, r, p;
		XMMatrixDecompose(&s, &r, &p, XMLoadFloat4x4(&world));
		XMStoreFloat3(&local_scale, s);
		XMStoreFloat4(&local_rotation, r);
		XMStoreFloat3(&local_position, p);
	}

	void TransformComponent::MatrixTransform(XMMATRIX matrix)
	{
		setDirty();
		XMVECTOR s, r, p;
		XMMatrixDecompose(&s, &r, &p, getLocalMatrix() * matrix);
		XMStoreFloat3(&local_scale, s);
		XMStoreFloat4(&local_rotation, r);
		XMStoreFloat3(&local_position, p);
	}

	void TransformComponent::updateTransform()
	{
		if (isDirty())
		{
			setDirty(false);
			XMStoreFloat4x4(&world, getLocalMatrix());
		}
	}

	void TransformComponent::updateTransformParented(TransformComponent* parent_transform)
	{
		XMStoreFloat4x4(&world, getLocalMatrix() * XMLoadFloat4x4(&parent_transform->world));
	}

	XMMATRIX TransformComponent::getLocalMatrix() const
	{
		XMVECTOR S_local = XMLoadFloat3(&local_scale);
		XMVECTOR R_local = XMLoadFloat4(&local_rotation);
		XMVECTOR T_local = XMLoadFloat3(&local_position);
		return
			XMMatrixScalingFromVector(S_local) *
			XMMatrixRotationQuaternion(R_local) *
			XMMatrixTranslationFromVector(T_local);
	}

	void TransformComponent::translate(const XMFLOAT3& move)
	{
		setDirty();
		local_position.x += move.x;
		local_position.y += move.y;
		local_position.z += move.z;
	}

	void TransformComponent::rotateRollPitchYaw(const XMFLOAT3& value)
	{
		setDirty();

		// This needs to be handled a bit differently
		XMVECTOR quat = XMLoadFloat4(&local_rotation);
		XMVECTOR x = XMQuaternionRotationRollPitchYaw(value.x, 0, 0);
		XMVECTOR y = XMQuaternionRotationRollPitchYaw(0, value.y, 0);
		XMVECTOR z = XMQuaternionRotationRollPitchYaw(0, 0, value.z);

		quat = XMQuaternionMultiply(x, quat);
		quat = XMQuaternionMultiply(quat, y);
		quat = XMQuaternionMultiply(z, quat);
		quat = XMQuaternionNormalize(quat);

		XMStoreFloat4(&local_rotation, quat);
	}

	void MeshComponent::deleteRenderData()
	{
		general_buffer = {};
		ib = {};
		vb_pos_wind = {};
	}

	void MeshComponent::createRenderData()
	{
		deleteRenderData();
		RHI* rhi = rhi::getRHI();
		GPUBufferDesc desc;
		desc.usage = Usage::DEFAULT;
		desc.bind_flags = BindFlag::VERTEX_BUFFER | BindFlag::INDEX_BUFFER | BindFlag::SHADER_RESOURCE;
		desc.misc_flags = ResourceMiscFlag::BUFFER_RAW | ResourceMiscFlag::NO_DEFAULT_DESCRIPTORS;
		const uint64_t alignment = rhi->getMinOffsetAlignment(&desc);
		const uint32_t position_stride = getFormatStride(position_format);
		const size_t uv_count = std::max(vertex_uvset_0.size(), vertex_uvset_1.size());
		desc.size = alignTo(vertex_positions.size() * position_stride, alignment) +
			alignTo(indices.size() * getIndexBufferStride(), alignment) +
			alignTo(vertex_colors.size() * sizeof(Vertex_Color), alignment) +
			alignTo(uv_count * sizeof(Vertex_UVS),alignment);

		XMFLOAT3 _min = XMFLOAT3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		XMFLOAT3 _max = XMFLOAT3(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
		for (int i = 0; i < vertex_positions.size(); ++i)
		{
			XMFLOAT3 pos = vertex_positions[i];
			_min = math::min(_min, pos);
			_max = math::max(_max, pos);
		}
		aabb = primitive::AABB(_min, _max);

		auto init_buffer_callback = [&](void* dest) {
			uint8_t* buffer_data = (uint8_t*)dest;
			uint64_t buffer_offset = 0ull;

			// vertexBuffer - POSITION + WIND:
			switch (position_format)
			{
			case Vertex_POS16::FORMAT:
			{
				vb_pos_wind.offset = buffer_offset;
				vb_pos_wind.size = vertex_positions.size() * sizeof(Vertex_POS16);
				Vertex_POS16* vertices = (Vertex_POS16*)(buffer_data + buffer_offset);
				buffer_offset += alignTo(vb_pos_wind.size, alignment);
				for (size_t i = 0; i < vertex_positions.size(); ++i)
				{
					XMFLOAT3 pos = vertex_positions[i];
					const uint8_t wind = vertex_windweights.empty() ? 0xFF : vertex_windweights[i];
					Vertex_POS16 vert;
					vert.fromFull(aabb, pos, wind);
					std::memcpy(vertices + i, &vert, sizeof(vert));
				}
			}
			break;
			case Vertex_POS32::FORMAT:
			{
				vb_pos_wind.offset = buffer_offset;
				vb_pos_wind.size = vertex_positions.size() * sizeof(Vertex_POS32);
				Vertex_POS32* vertices = (Vertex_POS32*)(buffer_data + buffer_offset);
				buffer_offset += alignTo(vb_pos_wind.size, alignment);
				for (size_t i = 0; i < vertex_positions.size(); ++i)
				{
					const XMFLOAT3& pos = vertex_positions[i];
					const uint8_t wind = vertex_windweights.empty() ? 0xFF : vertex_windweights[i];
					Vertex_POS32 vert;
					vert.fromFull(pos);
					std::memcpy(vertices + i, &vert, sizeof(vert));
				}
			}
			break;

			default:
				assert(0);
				break;
			}

			// Create index buffer GPU data:
			if (getIndexFormat() == IndexBufferFormat::UINT32)
			{
				ib.offset = buffer_offset;
				ib.size = indices.size() * sizeof(uint32_t);
				uint32_t* indexdata = (uint32_t*)(buffer_data + buffer_offset);
				buffer_offset += alignTo(ib.size, alignment);
				std::memcpy(indexdata, indices.data(), ib.size);
			}
			else
			{
				ib.offset = buffer_offset;
				ib.size = indices.size() * sizeof(uint16_t);
				uint16_t* indexdata = (uint16_t*)(buffer_data + buffer_offset);
				buffer_offset += alignTo(ib.size, alignment);
				for (size_t i = 0; i < indices.size(); ++i)
				{
					std::memcpy(indexdata + i, &indices[i], sizeof(uint16_t));
				}
			}

			//vertex buffer - UV Sets
			if (!vertex_uvset_0.empty() || !vertex_uvset_1.empty())
			{
				const XMFLOAT2* uv0_stream = vertex_uvset_0.empty() ? vertex_uvset_1.data() : vertex_uvset_0.data();
				const XMFLOAT2* uv1_stream = vertex_uvset_1.empty() ? vertex_uvset_0.data() : vertex_uvset_1.data();

				vb_uvs.offset = buffer_offset;
				vb_uvs.size = uv_count * sizeof(Vertex_UVS);
				Vertex_UVS* vertices = (Vertex_UVS*)(buffer_data + buffer_offset);
				buffer_offset += alignTo(vb_uvs.size, alignment);
				for (int i = 0; i < uv_count; ++i)
				{
					Vertex_UVS vert;
					vert.uv0.FromFULL(uv0_stream[i], uv_range_min, uv_range_max);
					vert.uv1.FromFULL(uv1_stream[i], uv_range_min, uv_range_max);
					std::memcpy(vertices + i, &vert, sizeof(vert));
				}
			}

			};

		bool success = rhi->createBuffer(&desc, &general_buffer, init_buffer_callback);
		assert(success);
		rhi->setName(&general_buffer, "general_buffer");

		assert(vb_pos_wind.IsValid());
		vb_pos_wind.subresource_srv = rhi->createSubresource(&general_buffer, SubresourceType::SRV, vb_pos_wind.offset, vb_pos_wind.size, &position_format);
		vb_pos_wind.descriptor_srv = rhi->getDescriptorIndex(&general_buffer, SubresourceType::SRV, vb_pos_wind.subresource_srv);
		assert(ib.IsValid());
		const Format ib_format = getIndexFormat() == IndexBufferFormat::UINT32 ? Format::R32_UINT : Format::R16_UINT;
		ib.subresource_srv = rhi->createSubresource(&general_buffer, SubresourceType::SRV, ib.offset, ib.size, &ib_format);
		ib.descriptor_srv = rhi->getDescriptorIndex(&general_buffer, SubresourceType::SRV, ib.subresource_srv);
		if (vb_uvs.IsValid())
		{
			vb_uvs.subresource_srv = rhi->createSubresource(&general_buffer, SubresourceType::SRV, vb_uvs.offset, vb_uvs.size, &Vertex_UVS::FORMAT);
			vb_uvs.descriptor_srv = rhi->getDescriptorIndex(&general_buffer, SubresourceType::SRV, vb_uvs.subresource_srv);
		}
	}

	void CameraComponent::transformCamera(const XMMATRIX& transform_matrix)
	{
		XMVECTOR _Eye = XMVector3Transform(XMVectorSet(0, 0, 0, 1), transform_matrix);
		XMVECTOR _At = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), transform_matrix));
		XMVECTOR _Up = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), transform_matrix));

		XMMATRIX _V = XMMatrixLookToLH(_Eye, _At, _Up);
		XMStoreFloat4x4(&view, _V);

		XMStoreFloat3x3(&rotation_matrix, XMMatrixInverse(nullptr, _V));

		XMStoreFloat3(&eye, _Eye);
		XMStoreFloat3(&look_at, _At);
		XMStoreFloat3(&up, _Up);
	}

	void CameraComponent::updateCamera()
	{

		XMMATRIX P;

		P = XMMatrixPerspectiveFovLH(fov, width / height, z_far, z_near); // reverse zbuffer!

		XMStoreFloat4x4(&proj, P);

		XMVECTOR _Eye = XMLoadFloat3(&eye);
		XMVECTOR _At = XMLoadFloat3(&look_at);
		XMVECTOR _Up = XMLoadFloat3(&up);
		/*std::cout << "eye:" << eye.x << "  " << eye.y << "  " << eye.z << std::endl;
		std::cout << "at:" << look_at.x << "  " << look_at.y << "  " << look_at.z << std::endl;
		std::cout << "up:" << up.x << "  " << up.y << "  " << up.z << std::endl;*/

		XMMATRIX _V = XMMatrixLookToLH(_Eye, _At, _Up);
		XMStoreFloat4x4(&view, _V);

		XMMATRIX _P = XMLoadFloat4x4(&proj);
		XMMATRIX _InvP = XMMatrixInverse(nullptr, _P);
		XMStoreFloat4x4(&inv_projection, _InvP);

		XMMATRIX _VP = XMMatrixTranspose(XMMatrixMultiply(_V, _P));
		XMStoreFloat4x4(&view, _V);
		XMStoreFloat4x4(&view_proj, _VP);
		XMMATRIX _InvV = XMMatrixInverse(nullptr, _V);
		XMStoreFloat4x4(&inv_view, _InvV);
		XMStoreFloat3x3(&rotation_matrix, _InvV);
		XMStoreFloat4x4(&inv_view_projection, XMMatrixInverse(nullptr, _VP));
	}

	void MaterialComponent::writeShaderMaterial(ShaderMaterial* dst)
	{
		RHI* rhi = rhi::getRHI();
		ShaderMaterial material;
		material.init();
		material.base_color = base_color;
		material.base_color = XMFLOAT4(1,1,1,1);
		for (int i = 0; i < TEXTURESLOT_COUNT; ++i)
		{
			const MaterialComponent::TextureMap& texture_map = textures[i];
			material.textures[i].uvset_aniso_lodclamp = (texture_map.uvset & 1);
			if (texture_map.resource.isValid())
			{
				int subresource = -1;
				switch (i)
				{
				case BASECOLORMAP:
					subresource = texture_map.resource.getTextureSRGBSubresource();
					break;
				default:
					break;
				}
				material.textures[i].texture_descriptor = rhi->getDescriptorIndex(texture_map.getGPUResource(), SubresourceType::SRV, subresource);
			}
			else
			{
				material.textures[i].texture_descriptor = -1;
			}
		}
		if (sampler_descriptor < 0)
		{
			material.sampler_descriptor = rhi->getDescriptorIndex(renderer::getSampler(enums::SAMPLER_OBJECTSHADER));
		}
		else
		{
			material.sampler_descriptor = sampler_descriptor;
		}
		std::memcpy(dst, &material, sizeof(material));
		
	}

	resourcemanager::Flags MaterialComponent::getTextureSlotResourceFlags(TEXTURESLOT slot)
	{
		resourcemanager::Flags flags = resourcemanager::Flags::NONE;
		if (!isPreferUnCompressedTexturesEnabled())
		{
			flags |= resourcemanager::Flags::IMPORT_BLOCK_COMPRESSED;
		}
		if (!isTextureStreamingDisabled())
		{
			flags |= resourcemanager::Flags::STREAMING;
		}
		return flags;
	}

	void MaterialComponent::createRenderData()
	{
		for (uint32_t i = 0; i < TEXTURESLOT_COUNT; ++i)
		{
			TextureMap & tex = textures[i];
			if (!tex.name.empty())
			{
				resourcemanager::Flags flags = getTextureSlotResourceFlags(TEXTURESLOT(i));
				tex.resource = resourcemanager::load(tex.name, flags);
			}
		}
	}

}
