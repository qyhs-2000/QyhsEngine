#include "model_importer_gltf.h"
#include "core/base/macro.h"
#include "function/global/global_context.h"
#include "function/render/render_type.h"
#include "function/render/render_system.h"
#include "function/framework/level/level.h"
#include "function/render/scene.h"
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#ifdef VK_USE_PLATFORM_ANDROID_KHR
#	define TINYGLTF_ANDROID_LOAD_FROM_ASSETS
#endif
#include "tinygltf/tiny_gltf.h"
#include "function/framework/component/mesh/mesh_component.h"
#include "function/framework/component/transform/transform_component.h"

#include "core/math/random.h"
#include "resource/resource_manager.h"
namespace tinygltf
{
	bool LoadImageData(Image* image, const int image_idx, std::string* err,
		std::string* warn, int req_width, int req_height,
		const unsigned char* bytes, int size, void* userdata)
	{
		if (image->uri.empty())
		{
			std::string ss;
			while (true)
			{
				ss.clear();
				ss += "gltfimport_" + std::to_string(qyhs::random::getRandom(std::numeric_limits<uint32_t>::max())) + ".png";
				if (!qyhs::resourcemanager::contains(ss))
				{
					image->uri = ss;
					break;
				}
			}

		}
		auto resource = qyhs::resourcemanager::load(image->uri, qyhs::resourcemanager::Flags::IMPORT_RETAIN_FILEDATA | qyhs::resourcemanager::Flags::IMPORT_DELAY,
			(const uint8_t*)bytes,
			(size_t)size);
		if (!resource.isValid())
		{
			return false;
		}
		qyhs::resourcemanager::ResourceSerializer* seri = (qyhs::resourcemanager::ResourceSerializer*)(userdata);
		seri->resources.push_back(resource);
		return true;
	}
}

namespace qyhs
{
	class LoaderState
	{
	public:
		GameObjectID root_gobject;
		std::unordered_map<int, GameObjectID> node_map_gobject;
		std::unordered_map<int, size_t> mesh_index_map_asset_id;
		std::unordered_map<int, size_t> material_index_map_asset_id;
		std::unordered_map<int, MeshComponent*> mesh_index_map_mesh_comp;
		std::shared_ptr<Level> level;
		tinygltf::Model model;
	private:
	};

	class LoaderState_Scene
	{
	public:
		Entity root_entity;
		std::unordered_map<int, Entity> entity_map;
		std::unordered_map<int, size_t> mesh_index_map_asset_id;
		std::unordered_map<int, size_t> material_index_map_asset_id;
		std::unordered_map<int, MeshComponent*> mesh_index_map_mesh_comp;
		scene::Scene* scene = nullptr;
		tinygltf::Model model;
	private:
	};

	void loadRenderMaterialData(RenderMaterialData& material_data, tinygltf::Image& image)
	{
		std::shared_ptr<TextureData> texture_data = std::make_shared<TextureData>();
		texture_data->m_width = image.width;
		texture_data->m_height = image.height;
		texture_data->m_pixels = image.image.data();
		switch (image.component)
		{
		case 3:
			texture_data->m_format = PIXEL_FORMAT::FORMAT_R8G8B8_SRGB;
			break;
		case 4:
			texture_data->m_format = PIXEL_FORMAT::FORMAT_R8G8B8A8_SRGB;
			break;
		default:
			texture_data->m_format = PIXEL_FORMAT::FORMAT_UNKNOWN;
			break;
		}
		material_data.m_base_color_texture = texture_data;
	}

	size_t createMaterial(tinygltf::Image& image, std::shared_ptr<RenderSystem>  render_system)
	{
		RenderMaterialData material_data;
		loadRenderMaterialData(material_data, image);
		return render_system->createMaterial(material_data);
	}

	void loadNode(int nodeIndex, GameObjectID parent, LoaderState_Scene& state)
	{
		if (nodeIndex < 0 || state.entity_map.count(nodeIndex) != 0)
		{
			return;
		}
		auto& node = state.model.nodes[nodeIndex];
		scene::Scene& scene = *state.scene;
		Entity entity = ecs::INVALID_ENTITY;

		if (node.mesh >= 0)
		{
			assert(node.mesh < (int)scene.meshes.getCount());

			if (node.skin >= 0)
			{
				// This node is an armature:
				entity = scene.armatures.getEntity(node.skin);
				scene::MeshComponent* mesh = &scene.meshes[node.mesh];
				Entity meshEntity = scene.meshes.getEntity(node.mesh);
				assert(!mesh->vertex_boneindices.empty());
				if (mesh->armatureID != ecs::INVALID_ENTITY)
				{
					// Reuse mesh with different skin is not possible currently, so we create a new one:
					meshEntity = entity;
					scene::MeshComponent& newMesh = scene.meshes.create(meshEntity);
					newMesh = scene.meshes[node.mesh];
					newMesh.createRenderData();
					mesh = &newMesh;
				}
				mesh->armatureID = entity;

				// The object component will use an identity transform but will be parented to the armature:
				Entity objectEntity = scene.createObjectEntity(node.name);
				scene::ObjectComponent& object = *scene.objects.getComponent(objectEntity);
				object.mesh_entity = meshEntity;
				scene.attachComponent(objectEntity, entity, true);
			}
			else
			{
				// This node is a mesh instance:
				entity = scene.createObjectEntity(node.name);
				scene::ObjectComponent& object = *scene.objects.getComponent(entity);
				object.mesh_entity = scene.meshes.getEntity(node.mesh);
			}
		}

		

		if (entity == ecs::INVALID_ENTITY)
		{
			entity = ecs::createEntity();
			scene.transforms.create(entity);
			scene.names.create(entity) = node.name;
		}

		state.entity_map[nodeIndex] = entity;

		scene::TransformComponent& transform = *scene.transforms.getComponent(entity);
		if (!node.scale.empty())
		{
			// Note: limiting min scale because scale <= 0.0001 will break matrix decompose and mess up the model (float precision issue?)
			for (int idx = 0; idx < 3; ++idx)
			{
				if (std::abs(node.scale[idx]) <= 0.0001)
				{
					const double sign = node.scale[idx] < 0 ? -1 : 1;
					node.scale[idx] = 0.0001001 * sign;
				}
			}
			transform.local_scale = XMFLOAT3(float(node.scale[0]), float(node.scale[1]), float(node.scale[2]));
		}
		if (!node.rotation.empty())
		{
			transform.local_rotation = XMFLOAT4((float)node.rotation[0], (float)node.rotation[1], (float)node.rotation[2], (float)node.rotation[3]);
		}
		if (!node.translation.empty())
		{
			transform.local_position = XMFLOAT3((float)node.translation[0], (float)node.translation[1], (float)node.translation[2]);
		}
		if (!node.matrix.empty())
		{
			transform.world._11 = (float)node.matrix[0];
			transform.world._12 = (float)node.matrix[1];
			transform.world._13 = (float)node.matrix[2];
			transform.world._14 = (float)node.matrix[3];
			transform.world._21 = (float)node.matrix[4];
			transform.world._22 = (float)node.matrix[5];
			transform.world._23 = (float)node.matrix[6];
			transform.world._24 = (float)node.matrix[7];
			transform.world._31 = (float)node.matrix[8];
			transform.world._32 = (float)node.matrix[9];
			transform.world._33 = (float)node.matrix[10];
			transform.world._34 = (float)node.matrix[11];
			transform.world._41 = (float)node.matrix[12];
			transform.world._42 = (float)node.matrix[13];
			transform.world._43 = (float)node.matrix[14];
			transform.world._44 = (float)node.matrix[15];
			transform.applyWorldToLocal(); // this creates S, R, T vectors from world matrix
		}

		transform.updateTransform();

		if (parent != ecs::INVALID_ENTITY)
		{
			scene.attachComponent(entity, parent, true);
		}

		if (!node.children.empty())
		{
			for (int child : node.children)
			{
				loadNode(child, entity, state);
			}
		}

	}

	void loadNode(int node_index, GameObjectID parent, LoaderState& loader_state)
	{
		if (node_index < 0)
		{
			return;
		}
		auto& node = loader_state.model.nodes[node_index];
		GameObjectID entity = k_invalid_gobject_id;

		//add mesh component
		if (node.mesh >= 0)
		{
			std::shared_ptr<GameObject> gobject = loader_state.level->createGameObject(node.name);
			entity = gobject->getObjectId();
			auto mesh_component = loader_state.mesh_index_map_mesh_comp[node.mesh];
			loader_state.level->getGameObjectByID(entity).lock()->TryAddComponentWithInstance(MeshComponent, mesh_component);
		}

		if (entity == k_invalid_gobject_id)
		{
			std::shared_ptr<GameObject> gobject = loader_state.level->createGameObject(node.name);
			entity = gobject->getObjectId();
		}
		std::shared_ptr<GameObject> gobject = loader_state.level->getGameObjectByID(entity).lock();
		loader_state.level->getGameObjectByID(entity).lock()->TryAddComponent(TransformComponent);

		TransformComponent* transform_component = loader_state.level->getGameObjectByID(entity).lock()->TryGetComponent(TransformComponent);
		if (!node.scale.empty())
		{
			for (int i = 0; i < 3; ++i)
			{
				if (std::abs(node.scale[i]) <= 0.0001)
				{
					const double sign = node.scale[i] < 0 ? -1 : 1;
					node.scale[i] = 0.0001001 * sign;
				}

			}
			transform_component->setScale(Vector3(node.scale[0], node.scale[1], node.scale[2]));
		}
		if (!node.rotation.empty())
		{
			Quaternion rotation;
			rotation.x = node.rotation[0];
			rotation.y = node.rotation[1];
			rotation.z = node.rotation[2];
			rotation.w = node.rotation[3];
			transform_component->setRotation(rotation);
		}
		if (!node.translation.empty())
		{
			Vector3 position;
			position.x = node.translation[0];
			position.y = node.translation[1];
			position.z = node.translation[2];
			transform_component->setPosition(position);
		}
		if (!node.matrix.empty())
		{
			transform_component->world_matrix = Matrix4x4(node.matrix[0], node.matrix[1], node.matrix[2], node.matrix[3],
				node.matrix[4], node.matrix[5], node.matrix[6], node.matrix[7],
				node.matrix[8], node.matrix[9], node.matrix[10], node.matrix[11],
				node.matrix[12], node.matrix[13], node.matrix[14], node.matrix[15]);
			transform_component->applyTransform();

		}

		transform_component->updateWorldMatrix();

		if (parent != k_invalid_gobject_id)
		{
			loader_state.level->attachGObject(entity, parent, true);
		}

		if (!node.children.empty())
		{
			for (auto& child : node.children)
			{
				loadNode(child, entity, loader_state);
			}
		}

	}

	bool import_model_gltf(std::shared_ptr<qyhs::Level> level, std::string file_path)
	{
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		LoaderState loader_state;
		std::string extension = Helper::toUpper(Helper::getFileExtension(file_path));
		std::vector<uint8_t> file_data;
		bool ret = Helper::fileRead(file_path, file_data);
		if (ret)
		{
			std::string basedir = tinygltf::GetBaseDir(file_path);

			if (!extension.compare("GLTF"))
			{
				ret = loader.LoadASCIIFromString(
					&loader_state.model,
					&err,
					&warn,
					reinterpret_cast<const char*>(file_data.data()),
					static_cast<unsigned int>(file_data.size()),
					basedir
				);
			}
			else
			{
				ret = loader.LoadBinaryFromMemory(
					&loader_state.model,
					&err,
					&warn,
					file_data.data(),
					static_cast<unsigned int>(file_data.size()),
					basedir
				);
			}
		}
		else
		{
			err = "Failed to read file: " + file_path;
		}

		auto root_object = level->createGameObject(Helper::getNameByFile(file_path));
		root_object->TryAddComponent(TransformComponent);
		loader_state.root_gobject = root_object->getObjectId();
		loader_state.level = level;


		std::shared_ptr<RenderSystem> p_render_system = g_runtime_global_context.m_render_system;
		std::shared_ptr<RenderScene> editor_render_scene = p_render_system->getRenderScene();

		//create material
		std::map<size_t, size_t> material_map_asset_id;
		for (size_t i = 0; i < loader_state.model.materials.size(); ++i)
		{
			auto& material = loader_state.model.materials[i];
			auto base_color_texture_iter = material.values.find("baseColorTexture");
			MaterialSourceDesc material_source_desc;

			if (base_color_texture_iter != material.values.end())
			{
				tinygltf::Texture texture = loader_state.model.textures[base_color_texture_iter->second.TextureIndex()];
				tinygltf::Image image = loader_state.model.images[texture.source];
				material_map_asset_id[i] = createMaterial(image, p_render_system);

			}

		}
		//create mesh
		for (int index = 0; index < loader_state.model.meshes.size(); ++index)
		{
			auto& mesh = loader_state.model.meshes[index];
			MeshComponent* mesh_component = new MeshComponent();
			loader_state.mesh_index_map_mesh_comp[index] = mesh_component;
			for (auto& primitive : mesh.primitives)
			{
				const size_t index_remap[] = {
					0,1,2
				};
				RenderMeshData mesh_data;
				if (primitive.indices >= 0)
				{
					const tinygltf::Accessor& accessor = loader_state.model.accessors[primitive.indices];
					const tinygltf::BufferView& buffer_view = loader_state.model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = loader_state.model.buffers[buffer_view.buffer];
					size_t stride = accessor.ByteStride(buffer_view);
					size_t index_buffer_data_size = accessor.count * sizeof(uint16_t);
					mesh_data.m_static_mesh_data.m_index_buffer = std::make_shared<BufferData>(index_buffer_data_size);
					std::shared_ptr<BufferData> index_buffer = mesh_data.m_static_mesh_data.m_index_buffer;
					const uint8_t* data = buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset;
					if (stride == 1)
					{
						for (size_t i = 0; i < accessor.count; i += 3)
						{
							uint16_t* p_data = (uint16_t*)index_buffer->m_data;
							p_data[i + 0] = data[i + index_remap[0]];
							p_data[i + 1] = data[i + index_remap[1]];
							p_data[i + 2] = data[i + index_remap[2]];
						}
					}
					else if (stride == 2)
					{
						for (size_t i = 0; i < accessor.count; i += 3)
						{
							uint16_t* p_data = (uint16_t*)index_buffer->m_data;
							p_data[i + 0] = ((uint16_t*)data)[i + index_remap[0]];
							p_data[i + 1] = ((uint16_t*)data)[i + index_remap[1]];
							p_data[i + 2] = ((uint16_t*)data)[i + index_remap[2]];
						}
					}
					else if (stride == 4)
					{
						for (size_t i = 0; i < accessor.count; i += 3)
						{
							uint16_t* p_data = (uint16_t*)index_buffer->m_data;
							p_data[i + 0] = ((uint32_t*)data)[i + index_remap[0]];
							p_data[i + 1] = ((uint32_t*)data)[i + index_remap[1]];
							p_data[i + 2] = ((uint32_t*)data)[i + index_remap[2]];
						}
					}
					else
					{
						assert(0 && "unsupported index stride!");
					}
				}


				for (auto& attribute : primitive.attributes)
				{
					const std::string& attr_name = attribute.first;
					const int attr_data = attribute.second;

					const tinygltf::Accessor& accessor = loader_state.model.accessors[attr_data];
					const tinygltf::BufferView& buffer_view = loader_state.model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = loader_state.model.buffers[buffer_view.buffer];

					int stride = accessor.ByteStride(buffer_view);
					int vertex_count = accessor.count;

					//no index buffer, so draw orderly
					if (mesh_data.m_static_mesh_data.m_index_buffer == nullptr)
					{
						mesh_data.m_static_mesh_data.m_index_buffer = std::make_shared<BufferData>(vertex_count * sizeof(uint16_t));
						uint16_t* p_index_buffer = (uint16_t*)(mesh_data.m_static_mesh_data.m_index_buffer->m_data);
						for (size_t i = 0; i < vertex_count; i += 3)
						{
							p_index_buffer[i + 0] = (uint16_t)(i + index_remap[0]);
							p_index_buffer[i + 1] = (uint16_t)(i + index_remap[1]);
							p_index_buffer[i + 2] = (uint16_t)(i + index_remap[2]);
						}
					}
					const uint8_t* data = buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset;
					if (!attr_name.compare("POSITION"))
					{
						uint32_t vertex_data_stride = sizeof(MeshVertexDataDefinition);
						if (mesh_data.m_static_mesh_data.m_vertex_buffer == nullptr)
						{
							mesh_data.m_static_mesh_data.m_vertex_buffer = std::make_shared<BufferData>(vertex_count * vertex_data_stride);
						}
						MeshVertexDataDefinition* p_vertex_buffer = (MeshVertexDataDefinition*)mesh_data.m_static_mesh_data.m_vertex_buffer->m_data;
						uint32_t stride = 3 * sizeof(float);
						for (size_t i = 0; i < vertex_count; ++i)
						{
							p_vertex_buffer[i].x = ((float*)(data + i * stride))[0];
							p_vertex_buffer[i].y = ((float*)(data + i * stride))[1];
							p_vertex_buffer[i].z = ((float*)(data + i * stride))[2];
						}
					}
					else if (!attr_name.compare("NORMAL"))
					{
						uint32_t vertex_data_stride = sizeof(MeshVertexDataDefinition);
						if (mesh_data.m_static_mesh_data.m_vertex_buffer == nullptr)
						{
							mesh_data.m_static_mesh_data.m_vertex_buffer = std::make_shared<BufferData>(vertex_count * vertex_data_stride);
						}
						MeshVertexDataDefinition* p_vertex_buffer = (MeshVertexDataDefinition*)mesh_data.m_static_mesh_data.m_vertex_buffer->m_data;
						uint32_t stride = 3 * sizeof(float);
						for (size_t i = 0; i < vertex_count; ++i)
						{
							p_vertex_buffer[i].nx = ((float*)(data + i * stride))[0];
							p_vertex_buffer[i].ny = ((float*)(data + i * stride))[1];
							p_vertex_buffer[i].nz = ((float*)(data + i * stride))[2];
						}
					}
					else if (!attr_name.compare("TANGENT"))
					{
						uint32_t vertex_data_stride = sizeof(MeshVertexDataDefinition);
						if (mesh_data.m_static_mesh_data.m_vertex_buffer == nullptr)
						{
							mesh_data.m_static_mesh_data.m_vertex_buffer = std::make_shared<BufferData>(vertex_count * vertex_data_stride);
						}
						MeshVertexDataDefinition* p_vertex_buffer = (MeshVertexDataDefinition*)mesh_data.m_static_mesh_data.m_vertex_buffer->m_data;
						uint32_t stride = 3 * sizeof(float);
						for (size_t i = 0; i < vertex_count; ++i)
						{
							p_vertex_buffer[i].tx = ((float*)(data + i * stride))[0];
							p_vertex_buffer[i].ty = ((float*)(data + i * stride))[1];
							p_vertex_buffer[i].tz = ((float*)(data + i * stride))[2];
						}
					}
					else if (!attr_name.compare("TEXCOORD_0"))
					{
						uint32_t vertex_data_stride = sizeof(MeshVertexDataDefinition);
						if (mesh_data.m_static_mesh_data.m_vertex_buffer == nullptr)
						{
							mesh_data.m_static_mesh_data.m_vertex_buffer = std::make_shared<BufferData>(vertex_count * vertex_data_stride);
						}
						MeshVertexDataDefinition* p_vertex_buffer = (MeshVertexDataDefinition*)mesh_data.m_static_mesh_data.m_vertex_buffer->m_data;
						uint32_t stride = 2 * sizeof(float);
						for (size_t i = 0; i < vertex_count; ++i)
						{
							p_vertex_buffer[i].u = ((float*)(data + i * stride))[0];
							p_vertex_buffer[i].v = ((float*)(data + i * stride))[1];
						}
					}
				}
				mesh_component->m_raw_meshes.push_back(GameObjectPartDesc());
				auto& game_object_part = mesh_component->m_raw_meshes.back();
				game_object_part.m_material_id = material_map_asset_id[primitive.material];
				size_t mesh_id = p_render_system->createMesh(mesh_data);
				game_object_part.m_mesh_id = mesh_id;
			}

		}

		const tinygltf::Scene& gltf_scene = loader_state.model.scenes[0 > loader_state.model.defaultScene ? 0 : loader_state.model.defaultScene];
		for (size_t i = 0; i < gltf_scene.nodes.size(); ++i)
		{
			loadNode(gltf_scene.nodes[i], loader_state.root_gobject, loader_state);
		}

		//bool is_entity_in_scene = false;
		//if (!is_entity_in_scene)
		//{
		//	editor_render_scene->m_render_entities.push_back(render_entity);
		//}
		//else
		//{
		//	for (auto& entity : editor_render_scene->m_render_entities)
		//	{
		//		if (entity.m_instance_id == render_entity.m_instance_id)
		//		{
		//			entity = render_entity;
		//			break;
		//		}
		//	}
		//}
		return true;

	}



	bool import_model_gltf(scene::Scene* scene, std::string file_path)
	{
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		LoaderState_Scene loader_state;
		std::string extension = Helper::toUpper(Helper::getFileExtension(file_path));

		resourcemanager::ResourceSerializer seri;
		loader.SetImageLoader(tinygltf::LoadImageData, &seri);
		std::vector<uint8_t> file_data;
		bool ret = Helper::fileRead(file_path, file_data);
		if (ret)
		{
			std::string basedir = tinygltf::GetBaseDir(file_path);

			if (!extension.compare("GLTF"))
			{
				ret = loader.LoadASCIIFromString(
					&loader_state.model,
					&err,
					&warn,
					reinterpret_cast<const char*>(file_data.data()),
					static_cast<unsigned int>(file_data.size()),
					basedir
				);
			}
			else
			{
				ret = loader.LoadBinaryFromMemory(
					&loader_state.model,
					&err,
					&warn,
					file_data.data(),
					static_cast<unsigned int>(file_data.size()),
					basedir
				);
			}
		}
		else
		{
			err = "Failed to read file: " + file_path;
		}

		std::string name = helper::getFileNameFromPath(file_path);
		loader_state.root_entity = ecs::createEntity();
		scene->transforms.create(loader_state.root_entity);
		scene->names.create(loader_state.root_entity) = name;
		loader_state.scene = scene;

		//create material
		for (auto& material : loader_state.model.materials)
		{
			ecs::Entity material_entity = scene->createMaterialEntity(material.name);
			scene->attachComponent(material_entity, loader_state.root_entity);

			scene::MaterialComponent& material_component = *scene->materials.getComponent(material_entity);
			material_component.base_color = XMFLOAT4(1, 1, 1, 1);
			auto base_color_texture_iter = material.values.find("baseColorTexture");
			auto base_color_factor = material.values.find("baseColorFactor");

			auto alpha_mode = material.additionalValues.find("alphaMode");
			if (base_color_factor != material.values.end())
			{
				material_component.base_color.x = float(base_color_factor->second.ColorFactor()[0]);
				material_component.base_color.y = float(base_color_factor->second.ColorFactor()[1]);
				material_component.base_color.z = float(base_color_factor->second.ColorFactor()[2]);
				material_component.base_color.w = float(base_color_factor->second.ColorFactor()[3]);
			}
			if (base_color_texture_iter != material.values.end())
			{
				auto& tex = loader_state.model.textures[base_color_texture_iter->second.TextureIndex()];
				int image_source = tex.source;
				auto& img = loader_state.model.images[image_source];
				material_component.textures[scene::MaterialComponent::BASECOLOR_MAP].name = img.uri;
				material_component.textures[scene::MaterialComponent::BASECOLOR_MAP].uvset = base_color_texture_iter->second.TextureTexCoord();
			}
			if (alpha_mode != material.additionalValues.end())
			{
				if (alpha_mode->second.string_value.compare("BLEND") == 0)
				{
					material_component.userBlendMode = enums::BLENDMODE_ALPHA;
				}
			}
			material_component.createRenderData();
		}

		//create mesh
		for (auto& x : loader_state.model.meshes)
		{
			Entity meshEntity = scene->createMeshEntity(x.name);
			scene->attachComponent(meshEntity, loader_state.root_entity);
			scene::MeshComponent& mesh = *scene->meshes.getComponent(meshEntity);

			for (auto& prim : x.primitives)
			{
				mesh.subsets.push_back(scene::MeshComponent::MeshSubset());
				if (scene->materials.getCount() == 0)
				{
					// Create a material last minute if there was none
					scene->materials.create(ecs::createEntity());
				}
				mesh.subsets.back().materialID = scene->materials.getEntity(std::max(0, prim.material));
				scene::MaterialComponent* material = scene->materials.getComponent(mesh.subsets.back().materialID);
				uint32_t vertexOffset = (uint32_t)mesh.vertex_positions.size();

				const size_t index_remap[] = {
					0,2,1
				};

				if (prim.indices >= 0)
				{
					// Fill indices:
					const tinygltf::Accessor& accessor = loader_state.model.accessors[prim.indices];
					const tinygltf::BufferView& bufferView = loader_state.model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = loader_state.model.buffers[bufferView.buffer];

					int stride = accessor.ByteStride(bufferView);
					size_t indexCount = accessor.count;
					size_t indexOffset = mesh.indices.size();
					mesh.indices.resize(indexOffset + indexCount);
					mesh.subsets.back().index_offset = (uint32_t)indexOffset;
					mesh.subsets.back().index_count = (uint32_t)indexCount;

					const uint8_t* data = buffer.data.data() + accessor.byteOffset + bufferView.byteOffset;

					if (stride == 1)
					{
						for (size_t i = 0; i < indexCount; i += 3)
						{
							mesh.indices[indexOffset + i + 0] = vertexOffset + data[i + index_remap[0]];
							mesh.indices[indexOffset + i + 1] = vertexOffset + data[i + index_remap[1]];
							mesh.indices[indexOffset + i + 2] = vertexOffset + data[i + index_remap[2]];
						}
					}
					else if (stride == 2)
					{
						for (size_t i = 0; i < indexCount; i += 3)
						{
							mesh.indices[indexOffset + i + 0] = vertexOffset + ((uint16_t*)data)[i + index_remap[0]];
							mesh.indices[indexOffset + i + 1] = vertexOffset + ((uint16_t*)data)[i + index_remap[1]];
							mesh.indices[indexOffset + i + 2] = vertexOffset + ((uint16_t*)data)[i + index_remap[2]];
						}
					}
					else if (stride == 4)
					{
						for (size_t i = 0; i < indexCount; i += 3)
						{
							mesh.indices[indexOffset + i + 0] = vertexOffset + ((uint32_t*)data)[i + index_remap[0]];
							mesh.indices[indexOffset + i + 1] = vertexOffset + ((uint32_t*)data)[i + index_remap[1]];
							mesh.indices[indexOffset + i + 2] = vertexOffset + ((uint32_t*)data)[i + index_remap[2]];
						}
					}
					else
					{
						assert(0 && "unsupported index stride!");
					}
				}

				for (auto& attr : prim.attributes)
				{
					const std::string& attr_name = attr.first;
					int attr_data = attr.second;

					const tinygltf::Accessor& accessor = loader_state.model.accessors[attr_data];
					const tinygltf::BufferView& bufferView = loader_state.model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = loader_state.model.buffers[bufferView.buffer];

					int stride = accessor.ByteStride(bufferView);
					size_t vertexCount = accessor.count;

					if (mesh.subsets.back().index_count == 0)
					{
						// Autogen indices:
						//	Note: this is not common, so it is simpler to create a dummy index buffer here than rewrite engine to support this case
						size_t indexOffset = mesh.indices.size();
						mesh.indices.resize(indexOffset + vertexCount);
						for (size_t vi = 0; vi < vertexCount; vi += 3)
						{
							mesh.indices[indexOffset + vi + 0] = uint32_t(vertexOffset + vi + index_remap[0]);
							mesh.indices[indexOffset + vi + 1] = uint32_t(vertexOffset + vi + index_remap[1]);
							mesh.indices[indexOffset + vi + 2] = uint32_t(vertexOffset + vi + index_remap[2]);
						}
						mesh.subsets.back().index_offset = (uint32_t)indexOffset;
						mesh.subsets.back().index_count = (uint32_t)vertexCount;
					}

					const uint8_t* data = buffer.data.data() + accessor.byteOffset + bufferView.byteOffset;

					if (!attr_name.compare("POSITION"))
					{
						mesh.vertex_positions.resize(vertexOffset + vertexCount);
						for (size_t i = 0; i < vertexCount; ++i)
						{
							mesh.vertex_positions[vertexOffset + i] = *(const XMFLOAT3*)(data + i * stride);
						}

						if (accessor.sparse.isSparse)
						{
							auto& sparse = accessor.sparse;
							const tinygltf::BufferView& sparse_indices_view = loader_state.model.bufferViews[sparse.indices.bufferView];
							const tinygltf::BufferView& sparse_values_view = loader_state.model.bufferViews[sparse.values.bufferView];
							const tinygltf::Buffer& sparse_indices_buffer = loader_state.model.buffers[sparse_indices_view.buffer];
							const tinygltf::Buffer& sparse_values_buffer = loader_state.model.buffers[sparse_values_view.buffer];
							const uint8_t* sparse_indices_data = sparse_indices_buffer.data.data() + sparse.indices.byteOffset + sparse_indices_view.byteOffset;
							const uint8_t* sparse_values_data = sparse_values_buffer.data.data() + sparse.values.byteOffset + sparse_values_view.byteOffset;
							switch (sparse.indices.componentType)
							{
							default:
							case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
								for (int s = 0; s < sparse.count; ++s)
								{
									mesh.vertex_positions[sparse_indices_data[s]] = ((const XMFLOAT3*)sparse_values_data)[s];
								}
								break;
							case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
								for (int s = 0; s < sparse.count; ++s)
								{
									mesh.vertex_positions[((const uint16_t*)sparse_indices_data)[s]] = ((const XMFLOAT3*)sparse_values_data)[s];
								}
								break;
							case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
								for (int s = 0; s < sparse.count; ++s)
								{
									mesh.vertex_positions[((const uint32_t*)sparse_indices_data)[s]] = ((const XMFLOAT3*)sparse_values_data)[s];
								}
								break;
							}
						}
					}
					else if (!attr_name.compare("NORMAL"))
					{
						mesh.vertex_normals.resize(vertexOffset + vertexCount);
						for (size_t i = 0; i < vertexCount; ++i)
						{
							mesh.vertex_normals[vertexOffset + i] = *(const XMFLOAT3*)(data + i * stride);
						}

						if (accessor.sparse.isSparse)
						{
							auto& sparse = accessor.sparse;
							const tinygltf::BufferView& sparse_indices_view = loader_state.model.bufferViews[sparse.indices.bufferView];
							const tinygltf::BufferView& sparse_values_view = loader_state.model.bufferViews[sparse.values.bufferView];
							const tinygltf::Buffer& sparse_indices_buffer = loader_state.model.buffers[sparse_indices_view.buffer];
							const tinygltf::Buffer& sparse_values_buffer = loader_state.model.buffers[sparse_values_view.buffer];
							const uint8_t* sparse_indices_data = sparse_indices_buffer.data.data() + sparse.indices.byteOffset + sparse_indices_view.byteOffset;
							const uint8_t* sparse_values_data = sparse_values_buffer.data.data() + sparse.values.byteOffset + sparse_values_view.byteOffset;
							switch (sparse.indices.componentType)
							{
							default:
							case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
								for (int s = 0; s < sparse.count; ++s)
								{
									mesh.vertex_normals[sparse_indices_data[s]] = ((const XMFLOAT3*)sparse_values_data)[s];
								}
								break;
							case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
								for (int s = 0; s < sparse.count; ++s)
								{
									mesh.vertex_normals[((const uint16_t*)sparse_indices_data)[s]] = ((const XMFLOAT3*)sparse_values_data)[s];
								}
								break;
							case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
								for (int s = 0; s < sparse.count; ++s)
								{
									mesh.vertex_normals[((const uint32_t*)sparse_indices_data)[s]] = ((const XMFLOAT3*)sparse_values_data)[s];
								}
								break;
							}
						}
					}
					else if (!attr_name.compare("TANGENT"))
					{
						mesh.vertex_tangents.resize(vertexOffset + vertexCount);
						for (size_t i = 0; i < vertexCount; ++i)
						{
							mesh.vertex_tangents[vertexOffset + i] = *(const XMFLOAT4*)(data + i * stride);
						}
					}
					else if (!attr_name.compare("TEXCOORD_0"))
					{
						mesh.vertex_uvset_0.resize(vertexOffset + vertexCount);
						if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
						{
							for (size_t i = 0; i < vertexCount; ++i)
							{
								const XMFLOAT2& tex = *(const XMFLOAT2*)((size_t)data + i * stride);

								mesh.vertex_uvset_0[vertexOffset + i].x = tex.x;
								mesh.vertex_uvset_0[vertexOffset + i].y = tex.y;
							}
						}
						else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
						{
							for (size_t i = 0; i < vertexCount; ++i)
							{
								const uint8_t& s = *(uint8_t*)((size_t)data + i * stride + 0);
								const uint8_t& t = *(uint8_t*)((size_t)data + i * stride + 1);

								mesh.vertex_uvset_0[vertexOffset + i].x = s / 255.0f;
								mesh.vertex_uvset_0[vertexOffset + i].y = t / 255.0f;
							}
						}
						else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
						{
							for (size_t i = 0; i < vertexCount; ++i)
							{
								const uint16_t& s = *(uint16_t*)((size_t)data + i * stride + 0 * sizeof(uint16_t));
								const uint16_t& t = *(uint16_t*)((size_t)data + i * stride + 1 * sizeof(uint16_t));

								mesh.vertex_uvset_0[vertexOffset + i].x = s / 65535.0f;
								mesh.vertex_uvset_0[vertexOffset + i].y = t / 65535.0f;
							}
						}
					}
					else if (!attr_name.compare("TEXCOORD_1"))
					{
						mesh.vertex_uvset_1.resize(vertexOffset + vertexCount);
						if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
						{
							for (size_t i = 0; i < vertexCount; ++i)
							{
								const XMFLOAT2& tex = *(const XMFLOAT2*)((size_t)data + i * stride);

								mesh.vertex_uvset_1[vertexOffset + i].x = tex.x;
								mesh.vertex_uvset_1[vertexOffset + i].y = tex.y;
							}
						}
						else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
						{
							for (size_t i = 0; i < vertexCount; ++i)
							{
								const uint8_t& s = *(uint8_t*)((size_t)data + i * stride + 0);
								const uint8_t& t = *(uint8_t*)((size_t)data + i * stride + 1);

								mesh.vertex_uvset_1[vertexOffset + i].x = s / 255.0f;
								mesh.vertex_uvset_1[vertexOffset + i].y = t / 255.0f;
							}
						}
						else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
						{
							for (size_t i = 0; i < vertexCount; ++i)
							{
								const uint16_t& s = *(uint16_t*)((size_t)data + i * stride + 0 * sizeof(uint16_t));
								const uint16_t& t = *(uint16_t*)((size_t)data + i * stride + 1 * sizeof(uint16_t));

								mesh.vertex_uvset_1[vertexOffset + i].x = s / 65535.0f;
								mesh.vertex_uvset_1[vertexOffset + i].y = t / 65535.0f;
							}
						}
					}
					else if (!attr_name.compare("JOINTS_0"))
					{
						mesh.vertex_boneindices.resize(vertexOffset + vertexCount);
						if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
						{
							struct JointTmp
							{
								uint8_t ind[4];
							};

							for (size_t i = 0; i < vertexCount; ++i)
							{
								const JointTmp& joint = *(const JointTmp*)(data + i * stride);

								mesh.vertex_boneindices[vertexOffset + i].x = joint.ind[0];
								mesh.vertex_boneindices[vertexOffset + i].y = joint.ind[1];
								mesh.vertex_boneindices[vertexOffset + i].z = joint.ind[2];
								mesh.vertex_boneindices[vertexOffset + i].w = joint.ind[3];
							}
						}
						else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
						{
							struct JointTmp
							{
								uint16_t ind[4];
							};

							for (size_t i = 0; i < vertexCount; ++i)
							{
								const JointTmp& joint = *(const JointTmp*)(data + i * stride);

								mesh.vertex_boneindices[vertexOffset + i].x = joint.ind[0];
								mesh.vertex_boneindices[vertexOffset + i].y = joint.ind[1];
								mesh.vertex_boneindices[vertexOffset + i].z = joint.ind[2];
								mesh.vertex_boneindices[vertexOffset + i].w = joint.ind[3];
							}
						}
						else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
						{
							struct JointTmp
							{
								uint32_t ind[4];
							};

							for (size_t i = 0; i < vertexCount; ++i)
							{
								const JointTmp& joint = *(const JointTmp*)(data + i * stride);

								mesh.vertex_boneindices[vertexOffset + i].x = joint.ind[0];
								mesh.vertex_boneindices[vertexOffset + i].y = joint.ind[1];
								mesh.vertex_boneindices[vertexOffset + i].z = joint.ind[2];
								mesh.vertex_boneindices[vertexOffset + i].w = joint.ind[3];
							}
						}
						else
						{
							assert(0);
						}
					}
					else if (!attr_name.compare("JOINTS_1"))
					{
						assert(0);
					}
					else if (!attr_name.compare("WEIGHTS_0"))
					{
						mesh.vertex_boneweights.resize(vertexOffset + vertexCount);
						if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
						{
							for (size_t i = 0; i < vertexCount; ++i)
							{
								mesh.vertex_boneweights[vertexOffset + i] = *(XMFLOAT4*)((size_t)data + i * stride);
							}
						}
						else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
						{
							for (size_t i = 0; i < vertexCount; ++i)
							{
								const uint8_t& x = *(uint8_t*)((size_t)data + i * stride + 0);
								const uint8_t& y = *(uint8_t*)((size_t)data + i * stride + 1);
								const uint8_t& z = *(uint8_t*)((size_t)data + i * stride + 2);
								const uint8_t& w = *(uint8_t*)((size_t)data + i * stride + 3);

								mesh.vertex_boneweights[vertexOffset + i].x = x / 255.0f;
								mesh.vertex_boneweights[vertexOffset + i].x = y / 255.0f;
								mesh.vertex_boneweights[vertexOffset + i].x = z / 255.0f;
								mesh.vertex_boneweights[vertexOffset + i].x = w / 255.0f;
							}
						}
						else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
						{
							for (size_t i = 0; i < vertexCount; ++i)
							{
								const uint16_t& x = *(uint8_t*)((size_t)data + i * stride + 0 * sizeof(uint16_t));
								const uint16_t& y = *(uint8_t*)((size_t)data + i * stride + 1 * sizeof(uint16_t));
								const uint16_t& z = *(uint8_t*)((size_t)data + i * stride + 2 * sizeof(uint16_t));
								const uint16_t& w = *(uint8_t*)((size_t)data + i * stride + 3 * sizeof(uint16_t));

								mesh.vertex_boneweights[vertexOffset + i].x = x / 65535.0f;
								mesh.vertex_boneweights[vertexOffset + i].x = y / 65535.0f;
								mesh.vertex_boneweights[vertexOffset + i].x = z / 65535.0f;
								mesh.vertex_boneweights[vertexOffset + i].x = w / 65535.0f;
							}
						}
					}
					
				}


				
			}

			mesh.createRenderData(); // tangents are generated inside if needed, which must be done before FlipZAxis!
		}

		// Create armatures:
		for (auto& skin : loader_state.model.skins)
		{
			ecs::Entity armatureEntity = ecs::createEntity();
			scene->names.create(armatureEntity) = skin.name;
			scene->transforms.create(armatureEntity);
			scene->attachComponent(armatureEntity, loader_state.root_entity);
			scene::ArmatureComponent& armature = scene->armatures.create(armatureEntity);

			if (skin.inverseBindMatrices >= 0)
			{
				const tinygltf::Accessor& accessor = loader_state.model.accessors[skin.inverseBindMatrices];
				const tinygltf::BufferView& bufferView = loader_state.model.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = loader_state.model.buffers[bufferView.buffer];
				armature.inverse_bind_matrices.resize(accessor.count);
				memcpy(armature.inverse_bind_matrices.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(XMFLOAT4X4));
			}
			else
			{
				assert(0);
			}
		}

		const tinygltf::Scene& gltf_scene = loader_state.model.scenes[0 > loader_state.model.defaultScene ? 0 : loader_state.model.defaultScene];
		for (size_t i = 0; i < gltf_scene.nodes.size(); ++i)
		{
			loadNode(gltf_scene.nodes[i], loader_state.root_entity, loader_state);
		}

		// Create armature-bone mappings:
		int armatureIndex = 0;
		for (auto& skin : loader_state.model.skins)
		{
			Entity armatureEntity = scene->armatures.getEntity(armatureIndex);
			scene::ArmatureComponent& armature = scene->armatures[armatureIndex++];

			const size_t jointCount = skin.joints.size();

			armature.bone_collection.resize(jointCount);

			// Create bone collection:
			for (size_t i = 0; i < jointCount; ++i)
			{
				int jointIndex = skin.joints[i];
				Entity boneEntity = loader_state.entity_map[jointIndex];

				armature.bone_collection[i] = boneEntity;

			}
		}

		// Create animations:
		for (auto& anim : loader_state.model.animations)
		{
			ecs::Entity entity = ecs::createEntity();
			scene->names.create(entity) = anim.name;
			scene->attachComponent(entity, loader_state.root_entity);
			scene::AnimationComponent& animationcomponent = scene->animations.create(entity);
			animationcomponent.samplers.resize(anim.samplers.size());
			animationcomponent.channels.resize(anim.channels.size());

			for (size_t i = 0; i < anim.samplers.size(); ++i)
			{
				auto& sam = anim.samplers[i];

				if (!sam.interpolation.compare("LINEAR"))
				{
					animationcomponent.samplers[i].mode = scene::AnimationComponent::AnimationSampler::Mode::LINEAR;
				}
				else if (!sam.interpolation.compare("STEP"))
				{
					animationcomponent.samplers[i].mode = scene::AnimationComponent::AnimationSampler::Mode::STEP;
				}
				else if (!sam.interpolation.compare("CUBICSPLINE"))
				{
					animationcomponent.samplers[i].mode = scene::AnimationComponent::AnimationSampler::Mode::CUBICSPLINE;
				}

				animationcomponent.samplers[i].data = ecs::createEntity();
				scene->attachComponent(animationcomponent.samplers[i].data, entity);
				scene::AnimationDataComponent& animationdata = scene->animation_datas.create(animationcomponent.samplers[i].data);

				// AnimationSampler input = keyframe times
				{
					const tinygltf::Accessor& accessor = loader_state.model.accessors[sam.input];
					const tinygltf::BufferView& bufferView = loader_state.model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = loader_state.model.buffers[bufferView.buffer];

					assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

					int stride = accessor.ByteStride(bufferView);
					size_t count = accessor.count;

					animationdata.keyframe_times.resize(count);

					const unsigned char* data = buffer.data.data() + accessor.byteOffset + bufferView.byteOffset;

					assert(stride == 4);

					for (size_t j = 0; j < count; ++j)
					{
						float time = ((float*)data)[j];
						animationdata.keyframe_times[j] = time;
						animationcomponent.start = std::min(animationcomponent.start, time);
						animationcomponent.end = std::max(animationcomponent.end, time);
					}

				}

				// AnimationSampler output = keyframe data
				{
					const tinygltf::Accessor& accessor = loader_state.model.accessors[sam.output];
					const tinygltf::BufferView& bufferView = loader_state.model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = loader_state.model.buffers[bufferView.buffer];

					int stride = accessor.ByteStride(bufferView);
					size_t count = accessor.count;

					const unsigned char* data = buffer.data.data() + accessor.byteOffset + bufferView.byteOffset;

					switch (accessor.type)
					{
					case TINYGLTF_TYPE_SCALAR:
					{
						assert(stride == sizeof(float));
						animationdata.keyframe_data.resize(count);
						for (size_t j = 0; j < count; ++j)
						{
							animationdata.keyframe_data[j] = ((float*)data)[j];
						}
					}
					break;
					case TINYGLTF_TYPE_VEC3:
					{
						assert(stride == sizeof(XMFLOAT3));
						animationdata.keyframe_data.resize(count * 3);
						for (size_t j = 0; j < count; ++j)
						{
							((XMFLOAT3*)animationdata.keyframe_data.data())[j] = ((XMFLOAT3*)data)[j];
						}
					}
					break;
					case TINYGLTF_TYPE_VEC4:
					{
						assert(stride == sizeof(XMFLOAT4));
						animationdata.keyframe_data.resize(count * 4);
						for (size_t j = 0; j < count; ++j)
						{
							((XMFLOAT4*)animationdata.keyframe_data.data())[j] = ((XMFLOAT4*)data)[j];
						}
					}
					break;
					default: assert(0); break;

					}

				}

			}

			for (size_t i = 0; i < anim.channels.size(); ++i)
			{
				auto& channel = anim.channels[i];

				animationcomponent.channels[i].target = loader_state.entity_map[channel.target_node];
				assert(channel.sampler >= 0);
				animationcomponent.channels[i].sampler_index = (uint32_t)channel.sampler;

				if (!channel.target_path.compare("scale"))
				{
					animationcomponent.channels[i].path = scene::AnimationComponent::AnimationChannel::Path::SCALE;
				}
				else if (!channel.target_path.compare("rotation"))
				{
					animationcomponent.channels[i].path = scene::AnimationComponent::AnimationChannel::Path::ROTATION;
				}
				else if (!channel.target_path.compare("translation"))
				{
					animationcomponent.channels[i].path = scene::AnimationComponent::AnimationChannel::Path::TRANSLATION;
				}
				else if (!channel.target_path.compare("weights"))
				{
					animationcomponent.channels[i].path = scene::AnimationComponent::AnimationChannel::Path::WEIGHTS;
				}
				else
				{
					animationcomponent.channels[i].path = scene::AnimationComponent::AnimationChannel::Path::UNKNOWN;
				}
			}

		}

		return true;
	}


}
