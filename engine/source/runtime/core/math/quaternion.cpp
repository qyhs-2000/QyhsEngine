#include "quaternion.h"
#include "matrix4.h"
namespace QYHS
{
	const Quaternion Quaternion::ZERO(0, 0, 0, 0);
	const Quaternion Quaternion::IDENTITY(1, 0, 0, 0);

    void Quaternion::toRotationMatrix(Matrix3x3& kRot) const
    {
        float fTx = x + x;   // 2x
        float fTy = y + y;   // 2y
        float fTz = z + z;   // 2z
        float fTwx = fTx * w; // 2xw
        float fTwy = fTy * w; // 2yw
        float fTwz = fTz * w; // 2z2
        float fTxx = fTx * x; // 2x^2
        float fTxy = fTy * x; // 2xy
        float fTxz = fTz * x; // 2xz
        float fTyy = fTy * y; // 2y^2
        float fTyz = fTz * y; // 2yz
        float fTzz = fTz * z; // 2z^2

        kRot[0][0] = 1.0f - (fTyy + fTzz); // 1 - 2y^2 - 2z^2
        kRot[0][1] = fTxy - fTwz;          // 2xy - 2wz
        kRot[0][2] = fTxz + fTwy;          // 2xz + 2wy
        kRot[1][0] = fTxy + fTwz;          // 2xy + 2wz
        kRot[1][1] = 1.0f - (fTxx + fTzz); // 1 - 2x^2 - 2z^2
        kRot[1][2] = fTyz - fTwx;          // 2yz - 2wx
        kRot[2][0] = fTxz - fTwy;          // 2xz - 2wy
        kRot[2][1] = fTyz + fTwx;          // 2yz + 2wx
        kRot[2][2] = 1.0f - (fTxx + fTyy); // 1 - 2x^2 - 2y^2
    }

    void Quaternion::toRotationMatrix(Matrix4x4& kRot) const
    {
        float fTx = x + x;   // 2x
        float fTy = y + y;   // 2y
        float fTz = z + z;   // 2z
        float fTwx = fTx * w; // 2xw
        float fTwy = fTy * w; // 2yw
        float fTwz = fTz * w; // 2z2
        float fTxx = fTx * x; // 2x^2
        float fTxy = fTy * x; // 2xy
        float fTxz = fTz * x; // 2xz
        float fTyy = fTy * y; // 2y^2
        float fTyz = fTz * y; // 2yz
        float fTzz = fTz * z; // 2z^2

        kRot[0][0] = 1.0f - (fTyy + fTzz); // 1 - 2y^2 - 2z^2
        kRot[0][1] = fTxy - fTwz;          // 2xy - 2wz
        kRot[0][2] = fTxz + fTwy;          // 2xz + 2wy
        kRot[0][3] = 0;
        kRot[1][0] = fTxy + fTwz;          // 2xy + 2wz
        kRot[1][1] = 1.0f - (fTxx + fTzz); // 1 - 2x^2 - 2z^2
        kRot[1][2] = fTyz - fTwx;          // 2yz - 2wx
        kRot[1][3] = 0;
        kRot[2][0] = fTxz - fTwy;          // 2xz - 2wy
        kRot[2][1] = fTyz + fTwx;          // 2yz + 2wx
        kRot[2][2] = 1.0f - (fTxx + fTyy); // 1 - 2x^2 - 2y^2
        kRot[2][3] = 0;
        kRot[3][0] = 0;
        kRot[3][1] = 0;
        kRot[3][2] = 0;
        kRot[3][3] = 1;
    }
}