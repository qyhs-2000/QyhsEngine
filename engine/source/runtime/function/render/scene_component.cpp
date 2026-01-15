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

	void TransformComponent::ClearTransform()
	{
		setDirty();
		local_scale = XMFLOAT3(1, 1, 1);
		local_rotation = XMFLOAT4(0, 0, 0, 1);
		local_position = XMFLOAT3(0, 0, 0);
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

	XMVECTOR TransformComponent::GetPositionV() const
	{
		return XMLoadFloat3((XMFLOAT3*)&world._41);
	}

	XMFLOAT3 TransformComponent::getPosition() const
	{
		return *((XMFLOAT3*)&world._41);
	}

	void TransformComponent::serialize(Archive& archive, ecs::EntitySerializer& seri)
	{
		if (archive.isReadMode())
		{
			archive >> _flag;
			archive >> local_scale;
			archive >> local_rotation;
			archive >> local_position;

			setDirty();
			updateTransform();
		}
		else
		{
			archive << _flag; // maybe not needed just for dirtiness, but later might come handy if we have more persistent flags
			archive << local_scale;
			archive << local_rotation;
			archive << local_position;
		}
	}

	void MeshComponent::deleteRenderData()
	{
		general_buffer = {};
		ib = {};
		vb_pos_wind = {};
	}

	void MeshComponent::createStreamOutRenderData()
	{
		RHI* rhi = rhi::getRHI();
		GPUBufferDesc desc;
		desc.usage = Usage::DEFAULT;
		desc.bind_flags = BindFlag::VERTEX_BUFFER | BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
		desc.misc_flags = ResourceMiscFlag::BUFFER_RAW | ResourceMiscFlag::TYPED_FORMAT_CASTING | ResourceMiscFlag::NO_DEFAULT_DESCRIPTORS;

		const uint64_t alignment = rhi->getMinOffsetAlignment(&desc) * sizeof(Vertex_POS32); // additional alignment for RGB32F
		desc.size =
			alignTo(vertex_positions.size() * sizeof(Vertex_POS32), alignment);

		bool success = rhi->createBuffer(&desc, nullptr, &streamout_buffer);
		assert(success);
		rhi->setName(&streamout_buffer, "MeshComponent::streamoutBuffer");

		uint64_t buffer_offset = 0ull;

		shader_output_pos.offset = buffer_offset;
		shader_output_pos.size = vertex_positions.size() * sizeof(Vertex_POS32);
		buffer_offset += alignTo(shader_output_pos.size, alignment);
		shader_output_pos.subresource_srv = rhi->createSubresource(&streamout_buffer, SubresourceType::SRV, shader_output_pos.offset, shader_output_pos.size, &Vertex_POS32::FORMAT);
		shader_output_pos.subresource_uav = rhi->createSubresource(&streamout_buffer, SubresourceType::UAV, shader_output_pos.offset, shader_output_pos.size); // UAV can't have RGB32_F format!
		shader_output_pos.descriptor_srv = rhi->getDescriptorIndex(&streamout_buffer, SubresourceType::SRV, shader_output_pos.subresource_srv);
		shader_output_pos.descriptor_uav = rhi->getDescriptorIndex(&streamout_buffer, SubresourceType::UAV, shader_output_pos.subresource_uav);

	}

	size_t MeshComponent::getBoneInfluencedCount() const
	{
		size_t influenced_count = 0;
		if (!vertex_boneindices.empty())
		{
			influenced_count++;
		}
		if (!vertex_boneindices2.empty())
		{
			influenced_count++;
		}
		return influenced_count;
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
			alignTo(uv_count * sizeof(Vertex_UVS), alignment) +
			alignTo(vertex_boneindices.size() * sizeof(Vertex_Bone), alignment) +
			alignTo(vertex_boneindices2.size() * sizeof(Vertex_Bone), alignment);

		XMFLOAT3 _min = XMFLOAT3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		XMFLOAT3 _max = XMFLOAT3(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
		for (int i = 0; i < vertex_positions.size(); ++i)
		{
			XMFLOAT3 pos = vertex_positions[i];
			_min = math::min(_min, pos);
			_max = math::max(_max, pos);
		}
		aabb = primitive::AABB(_min, _max);

		auto init_buffer_callback = [=](void* dest) {

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

			if (!vertex_boneindices.empty())
			{
				vb_bon.offset = buffer_offset;
				const size_t influence_div4 = getBoneInfluencedCount();
				vb_bon.size = (vertex_boneindices.size() + vertex_boneindices2.size()) * sizeof(Vertex_Bone);
				Vertex_Bone* vertices = (Vertex_Bone*)(buffer_data + buffer_offset);
				buffer_offset += alignTo(vb_bon.size, alignment);
				assert(vertex_boneindices.size() == vertex_boneweights.size()); // must have same number of indices as weights
				assert(vertex_boneindices2.empty() || vertex_boneindices2.size() == vertex_boneindices.size()); // if second influence stream exists, it must be as large as the first
				assert(vertex_boneindices2.size() == vertex_boneweights2.size()); // must have same number of indices as weights
				for (size_t i = 0; i < vertex_boneindices.size(); ++i)
				{
					// Normalize weights:
					//	Note: if multiple influence streams are present,
					//	we have to normalize them together, not separately
					float weights[8] = {};
					weights[0] = vertex_boneweights[i].x;
					weights[1] = vertex_boneweights[i].y;
					weights[2] = vertex_boneweights[i].z;
					weights[3] = vertex_boneweights[i].w;
					if (influence_div4 > 1)
					{
						weights[4] = vertex_boneweights2[i].x;
						weights[5] = vertex_boneweights2[i].y;
						weights[6] = vertex_boneweights2[i].z;
						weights[7] = vertex_boneweights2[i].w;
					}
					float sum = 0;
					for (auto& weight : weights)
					{
						sum += weight;
					}
					if (sum > 0)
					{
						const float norm = 1.0f / sum;
						for (auto& weight : weights)
						{
							weight *= norm;
						}
					}
					// Store back normalized weights:
					vertex_boneweights[i].x = weights[0];
					vertex_boneweights[i].y = weights[1];
					vertex_boneweights[i].z = weights[2];
					vertex_boneweights[i].w = weights[3];
					if (influence_div4 > 1)
					{
						vertex_boneweights2[i].x = weights[4];
						vertex_boneweights2[i].y = weights[5];
						vertex_boneweights2[i].z = weights[6];
						vertex_boneweights2[i].w = weights[7];
					}

					Vertex_Bone vert;
					vert.fromFull(vertex_boneindices[i], vertex_boneweights[i]);
					std::memcpy(vertices + (i * influence_div4 + 0), &vert, sizeof(vert));

					if (influence_div4 > 1)
					{
						vert.fromFull(vertex_boneindices2[i], vertex_boneweights2[i]);
						std::memcpy(vertices + (i * influence_div4 + 1), &vert, sizeof(vert));
					}
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

		if (vb_bon.IsValid())
		{
			vb_bon.subresource_srv = rhi->createSubresource(&general_buffer, SubresourceType::SRV, vb_bon.offset, vb_bon.size);
			vb_bon.descriptor_srv = rhi->getDescriptorIndex(&general_buffer, SubresourceType::SRV, vb_bon.subresource_srv);
		}

		if (!vertex_boneindices.empty())
		{
			createStreamOutRenderData();
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

	void CameraComponent::serialize(Archive& archive, ecs::EntitySerializer& seri)
	{
	}

	void MaterialComponent::writeShaderMaterial(ShaderMaterial* dst)
	{
		RHI* rhi = rhi::getRHI();
		ShaderMaterial material;
		material.init();
		material.base_color = base_color;
		material.base_color = XMFLOAT4(1, 1, 1, 1);
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
					assert(0);
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

	uint32_t MaterialComponent::getFilterMask() const
	{
		if (userBlendMode == BLENDMODE_OPAQUE)
		{
			return FILTER_OPAQUE;
		}
		return FILTER_TRANSPARENT;
	}

	void MaterialComponent::createRenderData()
	{
		for (uint32_t i = 0; i < TEXTURESLOT_COUNT; ++i)
		{
			TextureMap& tex = textures[i];
			if (!tex.name.empty())
			{
				resourcemanager::Flags flags = getTextureSlotResourceFlags(TEXTURESLOT(i));
				tex.resource = resourcemanager::load(tex.name, flags);
			}
		}
	}


	AnimationComponent::AnimationChannel::PathDataType AnimationComponent::AnimationChannel::getPathDataType() const
	{
		switch (path)
		{
		case scene::AnimationComponent::AnimationChannel::Path::TRANSLATION:
			return PathDataType::Float3;
		case scene::AnimationComponent::AnimationChannel::Path::ROTATION:
			return PathDataType::Float4;
		case scene::AnimationComponent::AnimationChannel::Path::SCALE:
			return PathDataType::Float3;
		case scene::AnimationComponent::AnimationChannel::Path::WEIGHTS:
			return PathDataType::Weights;
		default:
			assert(0);
			break;
		}
		return PathDataType::Event;
	}

	void ArmatureComponent::serialize(Archive& archive, ecs::EntitySerializer& seri)
	{
		if (archive.isReadMode())
		{


			size_t boneCount;
			archive >> boneCount;
			bone_collection.resize(boneCount);
			for (size_t i = 0; i < boneCount; ++i)
			{
				serializeEntity(archive, bone_collection[i], seri);
			}

			archive >> inverse_bind_matrices;



			ecs::Entity rootBoneID;
			serializeEntity(archive, rootBoneID, seri);

		}
		else
		{


			archive << bone_collection.size();
			for (size_t i = 0; i < bone_collection.size(); ++i)
			{
				ecs::Entity boneID = bone_collection[i];
				serializeEntity(archive, boneID, seri);
			}

			archive << inverse_bind_matrices;

			ecs::Entity rootBoneID;
			serializeEntity(archive, rootBoneID, seri);

		}
	}

	void AnimationComponent::serialize(Archive& archive, ecs::EntitySerializer& seri)
	{
		if (archive.isReadMode())
		{
			archive >> _flags;
			archive >> start;
			archive >> end;
			archive >> timer;
			archive >> amount;
			archive >> speed;
			archive >> name;

			size_t channelCount;
			archive >> channelCount;
			channels.resize(channelCount);
			for (size_t i = 0; i < channelCount; ++i)
			{

				archive >> (uint32_t&)channels[i].path;
				serializeEntity(archive, channels[i].target, seri);
				archive >> channels[i].sampler_index;

				archive >> channels[i].retargetIndex;

			}

			size_t samplerCount;
			archive >> samplerCount;
			samplers.resize(samplerCount);
			for (size_t i = 0; i < samplerCount; ++i)
			{

				archive >> (uint32_t&)samplers[i].mode;
				serializeEntity(archive, samplers[i].data, seri);

			}


		}
		else
		{
			archive << _flags;
			archive << start;
			archive << end;
			archive << timer;
			archive << amount;
			archive << speed;
			archive << name;

			archive << channels.size();
			for (size_t i = 0; i < channels.size(); ++i)
			{

				archive << (uint32_t&)channels[i].path;
				serializeEntity(archive, channels[i].target, seri);
				archive << channels[i].sampler_index;

				archive << channels[i].retargetIndex;

			}

			archive << samplers.size();
			for (size_t i = 0; i < samplers.size(); ++i)
			{

				archive << samplers[i].mode;
				serializeEntity(archive, samplers[i].data, seri);
			}


		}

		// Root Bone Name
		serializeEntity(archive, root_motion_bone, seri);

	}

}
