#pragma once

#include "core/meta/reflection/reflection.h"

namespace QYHS
{
	REFLECTION_TYPE(Vertex)
		CLASS(Vertex, Fields)
	{
	public:
		float px, py, pz;
		float u, v;
	private:
	};

	REFLECTION_TYPE(MeshData)
		CLASS(MeshData, Fields)
	{
	public:
		std::vector<Vertex>	vertex_buffer;
		std::vector<uint16_t>    index_buffer;
	private:
	};
}