#include "Vector2.h"

namespace Farlor
{
    class Matrix2x2
    {
    public:
        union
        {
            float m_data[4];
            Vector2 m_rows[2];
            struct
            {
                float m_r1c1;
                float m_r1c2;
                float m_r2c1;
                float m_r2c2;
            };
        };

        Matrix2x2();
    };
}
