#include "model_importer_gltf.h"
#include "core/base/macro.h"
#include "function/global/global_context.h"
#include "function/render/render_type.h"
#include "function/render/render_system.h"
#include "function/framework/level/level.h"
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#ifdef VK_USE_PLATFORM_ANDROID_KHR
#	define TINYGLTF_ANDROID_LOAD_FROM_ASSETS
#endif
#include "tinygltf/tiny_gltf.h"
#include "function/framework/component/mesh/mesh_component.h"
#include "function/framework/component/transform/transform_component.h"
namespace QYHS
{
	class LoaderState
	{
	public:
		GameObjectID root_gobject;
		std::unordered_map<int, GameObjectID> node_map_gobject;
		std::unordered_map<int, size_t> mesh_index_map_asset_id;
		std::unordered_map<int, size_t> material_index_map_asset_id;
		std::unordered_map<int, MeshComponent *> mesh_index_map_mesh_comp;
		std::shared_ptr<Level> level;
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
			loader_state.level->getGameObjectByID(entity).lock()->TryAddComponentWithInstance(MeshComponent,mesh_component);
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
			transform_component->world_matrix = Matrix4x4 (node.matrix[0],node.matrix[1],node.matrix[2],node.matrix[3],
				node.matrix[4],node.matrix[5],node.matrix[6],node.matrix[7],
				node.matrix[8],node.matrix[9],node.matrix[10],node.matrix[11],
				node.matrix[12],node.matrix[13],node.matrix[14],node.matrix[15]);
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

	bool import_model_gltf(std::shared_ptr<QYHS::Level> level, std::string file_path)
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
			MeshComponent * mesh_component = new MeshComponent();
			loader_state.mesh_index_map_mesh_comp[index] = mesh_component;
			for (auto& primitive : mesh.primitives)
			{
				const size_t index_remap[] = {
					0,2,1
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

	
}
