#pragma once

#include "core/meta/reflection/reflection.h"

namespace QYHS
{
	REFLECTION_TYPE(Vertex)
		CLASS(Vertex, Fields)
	{
	public:
		float px, py, pz;
		float nx, ny, nz;
		float tx, ty, tz;
		float u, v;
	private:
	};

	REFLECTION_TYPE(SkeletonBind)
		CLASS(SkeletonBind, Fields)
	{
	public:
		int index0;
		int index1;
		int index2;
		int index3;
		float weight0;
		float weight1;
		float weight2;
		float weight3;
	private:
	};

	REFLECTION_TYPE(MeshData)
		CLASS(MeshData, Fields)
	{
	public:
		std::vector<Vertex>	vertex_buffer;
		std::vector<uint16_t>    index_buffer;
		std::vector<SkeletonBind> bind;
	private:
	};
}