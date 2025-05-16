#pragma once
#include "runtime\resource\type\data\mash_data.h"

namespace qyhs
{
	template<>
	Json Serializer::write(const Vertex&instance);
	template<>
	Vertex & Serializer::read(const Json & json_context,Vertex & instance);
	template<>
	Json Serializer::write(const SkeletonBind&instance);
	template<>
	SkeletonBind & Serializer::read(const Json & json_context,SkeletonBind & instance);
	template<>
	Json Serializer::write(const MeshData&instance);
	template<>
	MeshData & Serializer::read(const Json & json_context,MeshData & instance);
}//namespace

