#include "rhi.h"

namespace qyhs::rhi
{
	RHI *& getRHI()
	{
		static RHI* rhi = nullptr;
		return rhi;
	}

	

}

namespace qyhs
{
	void RHI::updateBuffer(const GPUBuffer* buffer, const void* data, CommandList cmd, uint64_t size, uint64_t offset)
	{
		if (buffer == nullptr || data == nullptr)
			return;
		size = std::min(buffer->desc.size, size);
		if (size == 0)
			return;
		GPUAllocation allocation = allocateGPU(size, cmd);
		std::memcpy(allocation.data, data, size);
		copyBuffer(buffer, offset, &allocation.buffer, allocation.offset, size, cmd);
	}
	//temporary gpu buffer only valid for one frame
	RHI::GPUAllocation qyhs::RHI::allocateGPU(uint64_t size, CommandList& cmd)
	{
		GPUAllocation allocation;
		if (size <= 0)
		{
			return allocation;
		}
		GPUBufferAllocator& allocator = getFrameAllocator(cmd);
		const uint64_t free_size = allocator.buffer.desc.size - allocator.offset;
		if (size > free_size)
		{
			//create new buffer 
			GPUBufferDesc desc;
			desc.usage = Usage::UPLOAD;
			desc.bind_flags = BindFlag::CONSTANT_BUFFER | BindFlag::VERTEX_BUFFER | BindFlag::INDEX_BUFFER | BindFlag::SHADER_RESOURCE;
			desc.misc_flags = ResourceMiscFlag::BUFFER_RAW;
			allocator.alignment = getMinOffsetAlignment(&desc);
			desc.size = alignTo((allocator.buffer.desc.size + size) * 2, allocator.alignment);
			//TODO:it won't update descriptor set
			//desc.size = 1024;
			createBuffer(&desc, &allocator.buffer);
			setName(&allocator.buffer, "frame_allocator");
			allocator.offset = 0;
		}

		allocation.buffer = allocator.buffer;
		allocation.offset = allocator.offset;
		allocation.data = (void*)((size_t)allocator.buffer.mapped_data + allocator.offset);
		allocator.offset += alignTo(size, allocator.alignment);
		assert(allocation.isValid());
		return allocation;
	}
}
