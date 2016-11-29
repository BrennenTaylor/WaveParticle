#include <tclap\CmdLine.h>

#include "Core\Engine.h"

#include "Test.h"

#include <iostream>
#include <string>
#include <vector>

// #include <spdlog/spdlog.h>

#include "Util/Logger.h"

using namespace std;

using namespace Farlor;

using namespace TCLAP;

int main(int argc, char** argv)
{
    // TestRandomCrap();

    EngineConfig config = {};

    try
    {
        // Create the interpreter
        CmdLine cmdParser("Command line description message", ' ', "0.1");

        SwitchArg fullscreenSwitch("f","fullscreen","Run in fullscreen mode", cmdParser, false);
        SwitchArg vsyncSwitch("v","vsync","Run with vsync on", cmdParser, false);

        vector<string> allowedRenderApis;
        allowedRenderApis.push_back("d3d11");
        ValuesConstraint<string> allowedVals(allowedRenderApis);
        // Define a value argument and add it to the command line
        ValueArg<string> renderApiArg("r", "RenderDevice", "Render API to use", false,
            "d3d11", "string");
        cmdParser.add(renderApiArg);

        cmdParser.parse(argc, argv);

        config.m_fullscreen = fullscreenSwitch.getValue();
        config.m_vsync = vsyncSwitch.getValue();

        std::string renderApi = renderApiArg.getValue();

        cout << "Using Render API: " << renderApi << endl;
    }
    catch (ArgException &e)
    {
        cerr << "Error: " << e.error() << " for arg " << e.argId() << endl;
    }

    Engine GameEngine = Engine::Instatnce();

    // FARLOR_LOG_STARTUP_STDOUT()
    FARLOR_LOG_STARTUP_SIMPLE_FILE("C:/Log/Farlor.txt")

    FARLOR_LOG_INFO("Info")
    FARLOR_LOG_WARNING("Warning")
    FARLOR_LOG_ERROR("Error")
    FARLOR_LOG_CRITICAL("Critical")

    GameEngine.Initialize(config);

    GameEngine.Run();

    return 0;
}
