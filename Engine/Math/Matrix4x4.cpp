#include "Matrix4x4.h"

namespace Farlor
{
    Matrix4x4::Matrix4x4()
    {
        m_rows[0] = Vector4(0.0f);
        m_rows[1] = Vector4(0.0f);
        m_rows[2] = Vector4(0.0f);
        m_rows[3] = Vector4(0.0f);
    }

    Matrix4x4::Matrix4x4(FastMatrix& fastMatrix)
    {
        m_rows[0] = Vector4(fastMatrix.m_data[0], fastMatrix.m_data[1], fastMatrix.m_data[2], fastMatrix.m_data[3]);
        m_rows[1] = Vector4(fastMatrix.m_data[4], fastMatrix.m_data[5], fastMatrix.m_data[6], fastMatrix.m_data[7]);
        m_rows[2] = Vector4(fastMatrix.m_data[8], fastMatrix.m_data[9], fastMatrix.m_data[10], fastMatrix.m_data[11]);
        m_rows[3] = Vector4(fastMatrix.m_data[12], fastMatrix.m_data[13], fastMatrix.m_data[14], fastMatrix.m_data[15]);
    }

    std::ostream& operator<<(std::ostream& os, const Matrix4x4& mat)
    {
        os << "\n[" << mat.m_r1c1 << ", " << mat.m_r1c2 << ", " << mat.m_r1c3 << ", " << mat.m_r1c4 << "]" << std::endl;
        os << "[" << mat.m_r2c1 << ", " << mat.m_r2c2 << ", " << mat.m_r2c3 << ", " << mat.m_r2c4 << "]" << std::endl;
        os << "[" << mat.m_r3c1 << ", " << mat.m_r3c2 << ", " << mat.m_r3c3 << ", " << mat.m_r3c4 << "]" << std::endl;
        os << "[" << mat.m_r4c1 << ", " << mat.m_r4c2 << ", " << mat.m_r4c3 << ", " << mat.m_r4c4 << "]" << std::endl;
        return os;
    }
}
