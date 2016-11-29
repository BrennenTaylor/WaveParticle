#pragma once

#include "..\Farlor.h"

#include "EngineConfig.h"

namespace Farlor
{
    class Engine
    {
    private:
        Engine() : m_fullscreen{false}, m_vsync{false} {}

    public:
        virtual void Initialize(EngineConfig config);

        virtual int Run();

        static Engine& Instatnce()
        {
            static Engine instance;
            return instance;
        }

        private:
            bool m_fullscreen;
            bool m_vsync;
    };

    typedef Engine* EnginePtr;
}
