#pragma once
#include "runtime\resource\type\data\skeleton_data.h"

namespace qyhs
{
	template<>
	Json Serializer::write(const RawBone&instance);
	template<>
	RawBone & Serializer::read(const Json & json_context,RawBone & instance);
	template<>
	Json Serializer::write(const SkeletonData&instance);
	template<>
	SkeletonData & Serializer::read(const Json & json_context,SkeletonData & instance);
}//namespace

