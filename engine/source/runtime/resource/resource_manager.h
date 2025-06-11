#pragma once
#include <string>
#include <mutex>
#include <unordered_map>
#include "core/common_include.h"
#include "function/render/rhi/graphics.h"
#include <unordered_set>
#include "function/file/archive.h"
namespace qyhs::resourcemanager
{
	static std::mutex locker;

	enum class Flags
	{
		NONE = 0,
		IMPORT_DELAY = 1 << 0,
		IMPORT_BLOCK_COMPRESSED = 1 << 1,
		STREAMING = 1 << 2,
		IMPORT_RETAIN_FILEDATA = 1 << 3
	};

	struct Resource
	{
		std::shared_ptr<void> internal_state;
		int getTextureSRGBSubresource()const;
		const graphics::Texture& getTexture()const;
		inline bool isValid() const
		{
			return internal_state.get() != nullptr;
		}
	};

	struct ResourceSerializer
	{
		std::vector<Resource> resources;
	};

	Resource load(const std::string& name, Flags flags = Flags::NONE, const uint8_t* filedata = nullptr,
		size_t filesize = ~0ull,
		const std::string& container_filename = "",
		size_t container_fileoffset = 0);
	bool contains(std::string name);
	void Serialize_WRITE(Archive& archive, const std::unordered_set<std::string>& resource_names);
	void Serialize_READ(Archive& archive, ResourceSerializer & seri);
}

template<>
struct enable_bitmask_operators<qyhs::resourcemanager::Flags> {
	static const bool enable = true;
};
