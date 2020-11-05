#include <tclap\CmdLine.h>

#include "Core/Engine.h"
#include "Util/Logger.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
    Farlor::EngineConfig config = {};

    // TODO: Remove tclap and use something else. We want to not have exceptions here.
    try
    {
        // Create the interpreter
        TCLAP::CmdLine cmdParser("Command line description message", ' ', "0.1");

        TCLAP::SwitchArg fullscreenSwitch("f","fullscreen","Run in fullscreen mode", cmdParser, false);
        TCLAP::SwitchArg vsyncSwitch("v","vsync","Run with vsync on", cmdParser, false);

        std::vector<std::string> allowedRenderApis;
        allowedRenderApis.push_back("d3d11");
        TCLAP::ValuesConstraint<std::string> allowedVals(allowedRenderApis);
        // Define a value argument and add it to the command line
        TCLAP::ValueArg<std::string> renderApiArg("r", "RenderDevice", "Render API to use", false,
            "d3d11", "string");
        cmdParser.add(renderApiArg);

        cmdParser.parse(argc, argv);

        config.m_fullscreen = fullscreenSwitch.getValue();
        config.m_vsync = vsyncSwitch.getValue();

        std::string renderApi = renderApiArg.getValue();

        std::cout << "Using Render API: " << renderApi << std::endl;
    }
    catch (TCLAP::ArgException &e)
    {
        std::cout << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
    }

    // TODO: Should this be global static instance?
    Farlor::Engine gameEngine = Farlor::Engine::Instatnce();

    if (!std::filesystem::exists("E:/Log/"))
    {
        std::filesystem::create_directories("E:/Log/");
    }
    FARLOR_LOG_STARTUP_SIMPLE_FILE("E:/Log/WaveParticle.txt")

    FARLOR_LOG_INFO("Info")
    FARLOR_LOG_WARNING("Warning")
    FARLOR_LOG_ERROR("Error")
    FARLOR_LOG_CRITICAL("Critical")

    gameEngine.Initialize(config);
    gameEngine.Run();

    return 0;
}
