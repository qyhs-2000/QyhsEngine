#pragma once
#include "runtime\resource\type\data\mash_data.h"

namespace QYHS
{
	template<>
	Json Serializer::write(const Vertex&instance);
	template<>
	Vertex & Serializer::read(const Json & json_context,Vertex & instance);
	template<>
	Json Serializer::write(const MeshData&instance);
	template<>
	MeshData & Serializer::read(const Json & json_context,MeshData & instance);
}//namespace

