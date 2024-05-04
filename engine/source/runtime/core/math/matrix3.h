#pragma once
#include "core/meta/reflection/reflection.h"
namespace QYHS
{
	REFLECTION_TYPE(Matrix3x3)
	CLASS(Matrix3x3,Fields)
	{
	public:
		float m_mat[3][3];
		float* operator[](size_t row_index) const { return (float*)m_mat[row_index]; }
		
	private:
	};
}