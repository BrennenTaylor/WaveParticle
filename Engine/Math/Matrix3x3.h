#include "Vector3.h"

namespace Farlor
{
    class Matrix3x3
    {
    public:
        union
        {
            float m_data[9];
            Vector3 m_rows[3];
            struct
            {
                float m_r1c1;
                float m_r1c2;
                float m_r1c3;
                float m_r2c1;
                float m_r2c2;
                float m_r2c3;
                float m_r3c1;
                float m_r3c2;
                float m_r3c3;
            };
        };

        Matrix3x3();
    };
}
