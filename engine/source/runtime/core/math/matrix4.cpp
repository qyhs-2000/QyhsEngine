#include "matrix4.h"

namespace QYHS
{
    const Matrix4x4 Matrix4x4::ZERO(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    const Matrix4x4 Matrix4x4::ZEROAFFINE(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);

    const Matrix4x4 Matrix4x4::IDENTITY(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

    void Matrix4x4::makeTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation)
    {
        // Ordering:
        //    1. Scale
        //    2. Rotate
        //    3. Translate

        Matrix3x3 rot3x3;
        orientation.toRotationMatrix(rot3x3);

        // Set up final matrix with scale, rotation and translation
        m_mat[0][0] = scale.x * rot3x3[0][0];
        m_mat[0][1] = scale.y * rot3x3[0][1];
        m_mat[0][2] = scale.z * rot3x3[0][2];
        m_mat[0][3] = position.x;
        m_mat[1][0] = scale.x * rot3x3[1][0];
        m_mat[1][1] = scale.y * rot3x3[1][1];
        m_mat[1][2] = scale.z * rot3x3[1][2];
        m_mat[1][3] = position.y;
        m_mat[2][0] = scale.x * rot3x3[2][0];
        m_mat[2][1] = scale.y * rot3x3[2][1];
        m_mat[2][2] = scale.z * rot3x3[2][2];
        m_mat[2][3] = position.z;

        // No projection term
        m_mat[3][0] = 0;
        m_mat[3][1] = 0;
        m_mat[3][2] = 0;
        m_mat[3][3] = 1;
    }

    void Matrix4x4::decomposition(Vector3& position, Vector3& scale, Quaternion& orientation) const
    {
        Matrix3x3 matrix3x3;
        extract3x3Matrix(matrix3x3);

        Matrix3x3 mat_q;
        Vector3 vec_u;
        matrix3x3.calculateQDUDecomposition(mat_q,scale,vec_u);

        orientation = Quaternion(mat_q);
        position = Vector3(m_mat[0][3],m_mat[1][3],m_mat[2][3]);
    }

}