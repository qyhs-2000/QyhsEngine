#include "resource_manager.h"
#include "core/helper.h"
#include "function/render/rhi/rhi.h"
#include <stb_image.h>
namespace qyhs::resourcemanager
{
	struct ResourceInternal
	{
		Texture texture;
		Flags flags = Flags::NONE;
		int srgb_subresource = -1;
		std::vector<uint8_t> filedata;
		uint64_t timestamp = 0;
		std::string filename;
		std::string container_filename;
		size_t container_fileoffset = 0;
	};
	enum class DataType
	{
		IMAGE
	};
	std::unordered_map<std::string, std::weak_ptr<ResourceInternal>> resources;
	static std::unordered_map<std::string, DataType> types = {
		{"PNG",DataType::IMAGE}
	};
	bool loadResourceDirectly(std::string name, resourcemanager::Flags flags, const uint8_t* filedata, size_t filesize, ResourceInternal* resource)
	{
		std::string ext = helper::toUpper(helper::getExtensionFromFileName(name));
		DataType data_type;
		auto it = types.find(ext);
		if (it == types.end())
		{
			return false;
		}
		RHI* rhi = rhi::getRHI();
		data_type = it->second;
		bool success = false;
		switch (data_type)
		{
		case DataType::IMAGE:
		{
			//for png
			flags &= ~Flags::STREAMING;
			int width = 0, height = 0, channels = 0;
			graphics::Format format = graphics::Format::R8G8B8A8_UNORM;
			void* rgba;
			rgba = stbi_load_from_memory(filedata, (int)filesize, &width, &height, &channels, 0);
			Swizzle swizzle = { ComponentSwizzle::R, ComponentSwizzle::G, ComponentSwizzle::B, ComponentSwizzle::A };
			switch (channels)
			{
			case 1:
				format = graphics::Format::R8_UNORM;
				swizzle = { ComponentSwizzle::R, ComponentSwizzle::R, ComponentSwizzle::R, ComponentSwizzle::R };
				break;
			case 2:
				format = graphics::Format::R8G8_UNORM;
				swizzle = { ComponentSwizzle::R, ComponentSwizzle::R, ComponentSwizzle::R, ComponentSwizzle::A };
				break;
			case 3:
			{
				// graphics api doesn't support 3 channels,need to expend to rgba
				struct Color3
				{
					uint8_t r, g, b;
				};
				const Color3* color3 = (const Color3*)rgba;
				Color* color4 = (Color*)malloc(width * height * sizeof(Color));
				for (int i = 0; i < width * height; ++i)
				{
					color4[i].setR(color3[i].r);
					color4[i].setG(color3[i].g);
					color4[i].setB(color3[i].b);
					color4[i].setA(255);
				}
				free(rgba);
				rgba = color4;
				format = graphics::Format::R8G8B8A8_UNORM;
				swizzle = { ComponentSwizzle::R, ComponentSwizzle::G, ComponentSwizzle::B, ComponentSwizzle::A };
			}
			break;
			case 4:
				format = graphics::Format::R8G8B8A8_UNORM;
				swizzle = { ComponentSwizzle::R, ComponentSwizzle::G, ComponentSwizzle::B, ComponentSwizzle::A };
				break;
			default:
				break;
			}
			if (rgba != nullptr)
			{
				TextureDesc desc;
				desc.width = uint32_t(width);
				desc.height = uint32_t(height);
				desc.layout = ResourceState::SHADER_RESOURCE;
				desc.format = format;
				desc.swizzle = swizzle;

				desc.bind_flags = BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
				desc.mip_levels = getMipCount(width, height);
				desc.usage = Usage::DEFAULT;
				desc.misc_flags = ResourceMiscFlag::TYPED_FORMAT_CASTING;

				uint32_t mipwidth = width;
				SubresourceData init_data[16];
				for (uint32_t mip = 0; mip < desc.mip_levels; ++mip)
				{
					init_data[mip].data_ptr = rgba; // attention! we don't fill the mips here correctly, just always point to the mip0 data by default. Mip levels will be created using compute shader when needed!
					init_data[mip].row_pitch = uint32_t(mipwidth * getFormatStride(desc.format));
					mipwidth = std::max(1u, mipwidth / 2);
				}

				success = rhi->createTexture(&desc, &resource->texture, init_data);
				rhi->setName(&resource->texture, name.c_str());
				for (uint32_t i = 0; i < resource->texture.desc.mip_levels; ++i)
				{
					int subresource_index;
					subresource_index = rhi->createSubresource(&resource->texture, SubresourceType::SRV, 0, 1, i, 1);
					assert(subresource_index == i);
					subresource_index = rhi->createSubresource(&resource->texture, SubresourceType::UAV, 0, 1, i, 1);
					assert(subresource_index == i);
				}

				// This part must be AFTER mip level subresource creation:
				Format srgb_format = graphics::getFormatSRGB(desc.format);
				if (srgb_format != Format::UNKNOWN && srgb_format != desc.format)
				{
					resource->srgb_subresource = rhi->createSubresource(
						&resource->texture,
						SubresourceType::SRV,
						0, -1,
						0, -1,
						&srgb_format
					);
				}
				free(rgba);
			}
		}
		break;
		default:
			break;
		}
	}
	Resource load(const std::string& name, Flags flags, const uint8_t* filedata,
		size_t filesize,
		const std::string& container_filename,
		size_t container_fileoffset)
	{
		locker.lock();
		std::shared_ptr<ResourceInternal> resource = resources[name].lock();
		uint64_t timestamp = helper::fileTimeStamp(name);

		if (resource == nullptr || resource->timestamp < timestamp)
		{
			resource = std::make_shared<ResourceInternal>();
			resources[name] = resource;
			resource->filename = name;
			if (container_filename.empty())
			{
				resource->container_filename = name;
			}
			else
			{
				resource->container_filename = container_filename;
			}
			if (filedata != nullptr && resource->filedata.empty() && (has_flag(flags, Flags::IMPORT_RETAIN_FILEDATA)))
			{
				resource->filedata.resize(filesize);
				std::memcpy(resource->filedata.data(), filedata, filesize);
			}
		}
		else
		{
			if (!has_flag(flags, Flags::IMPORT_DELAY) && has_flag(resource->flags, Flags::IMPORT_DELAY))
			{
				resource->flags &= ~Flags::IMPORT_DELAY;
			}
		}

		locker.unlock();

		if (filedata == nullptr || filesize == 0)
		{
			if (resource->filedata.empty())
			{
				if (!helper::readFile(resource->container_filename, resource->filedata, resource->container_fileoffset));
				{
					resource.reset();
					return Resource();
				}
			}
			filedata = resource->filedata.data();
			filesize = resource->filedata.size();
		}
		flags |= resource->flags;

		bool success = false;

		if (has_flag(flags, Flags::IMPORT_DELAY))
		{
			success = true;
		}
		else
		{
			success = loadResourceDirectly(name, flags, filedata, filesize, resource.get());
		}

		if (success)
		{
			resource->flags = flags;
			resource->timestamp = timestamp;

			Resource retVal;
			retVal.internal_state = resource;
			return retVal;
		}

		return Resource();

	}

	bool contains(std::string name)
	{
		bool result = false;
		locker.lock();
		auto it = resources.find(name);
		if (it != resources.end())
		{
			auto resource = it->second.lock();
			result = resource != nullptr;
		}
		locker.unlock();
		return result;
	}
	int Resource::getTextureSRGBSubresource() const
	{
		const ResourceInternal* resourceinternal = (ResourceInternal*)internal_state.get();
		return resourceinternal->srgb_subresource;
	}
	const Texture& Resource::getTexture() const
	{
		const ResourceInternal *internal_resource = (ResourceInternal*)internal_state.get();
		return internal_resource->texture;
	}
}