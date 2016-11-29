#pragma once

namespace Farlor
{
    // Contains all information to pass to an engine for
    // startup which comes from the command line.
    struct EngineConfig
    {
        bool m_fullscreen;
        bool m_vsync;
    };
}