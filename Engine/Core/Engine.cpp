#define WIN32_LEAN_AND_MEAN

#include "../Farlor.h"

#include "Engine.h"
#include "Window.h"
#include "Timer.h"

#include "../Platform/Platform.h"

#include "../Util/HashedString.h"

#include "../Input/InputHandler.h"
#include "../Input/ControllerManager.h"

#include "Events/EventManager.h"

#include "../ECS/EntityManager.h"
#include "../EngineExport.h"

// Memory Stuff
#include "Memory/LinearAllocator.h"
#include "FixedUpdate.h"

// ************ GPU Pro 3: Designing a Data-Driven Renderer******
// Look up for rendering engine design advice

#include "../IGame.h"

#include "../ResourceManager/ResourceManager.h"

#include "../Physics/PhysicsSystem.h"

#include "../NewRenderer/Renderer.h"
#include "../NewRenderer/Camera.h"
#include "../NewRenderer/RenderingComponent.h"

#include "../Networking/NetworkManager.h"
#include "../ECS/TransformManager.h"

#include "../JsonCpp/json.h"

#include "../Util/Logger.h"

using namespace std;

namespace Farlor
{
    typedef GameExport* (*GetGameAPIPtr)(LinearAllocator& allocator, EngineExport& engineExport);

    // Global Declarations of engine systems
    EventManager g_EventManager{};
    PhysicsSystem g_PhysicsSystem{};
    Renderer g_RenderingSystem{};
    Camera g_MainCamera{true};
    Farlor::Timer g_TimerGame{};
    NetworkManager g_NetworkManager{};
    InputHandler g_InputHandler;
    TransformManager g_TransformManager{};
    GameWindow m_gameWindow;


    ParticleSystem g_WaveParticles{5000};
    Camera g_WaveParticleCamera{false};

    const float SCREEN_DEPTH = 1000.0f;
    const float SCREEN_NEAR = 0.001f;

    void Engine::Initialize(EngineConfig config)
    {
        m_fullscreen = config.m_fullscreen;
        m_vsync = config.m_vsync;

        // Initialize random timer
        srand ((unsigned int)time(0));

        // g_WaveParticles.AddPlane(Plane(-1.0f, 0.0f, 0.0f, 140.0, 0.0, 0.0));
        // g_WaveParticles.AddPlane(Plane(1.0f, 0.0f, 0.0f, -140.0, 0.0, 0.0));
        g_WaveParticles.AddPlane(Plane(0.0f, -1.0f, 0.0f, 0.0, 140.0, 0.0));
        g_WaveParticles.AddPlane(Plane(0.0f, 1.0f, 0.0f, 0.0, -140.0, 0.0));
        // g_WaveParticles.AddSegments(LineSegment(-50.0f, 0.0f, -20.0f, 50.0f));
        // g_WaveParticles.AddSegments(LineSegment(30.0f, 50.0f, 50.0f, 50.0f));
        g_WaveParticles.AddSegments(LineSegment(140.0f, 30.0f, 0.0f, 30.0f));
        // g_WaveParticles.AddSegments(LineSegment(-140.0f, 30.0f, -30.0f, 30.0f));

        g_WaveParticleCamera.m_camPosition = XMVectorSet(0.0f, 0.0f, -200.0f, 0.0f);
    }

    int Engine::Run()
    {
        // Create and show the game window
        // GameWindow m_gameWindow;
        m_gameWindow.Initialize(m_fullscreen);
        m_gameWindow.ShowGameWindow();

        g_RenderingSystem.Initialize(m_gameWindow);

        RAWINPUTDEVICE rawInputDevices[2];
        // Keyboard
        rawInputDevices[0].usUsagePage = 0x01;
        rawInputDevices[0].usUsage = 0x06; // Keyboard
        rawInputDevices[0].dwFlags = RIDEV_NOLEGACY; // Dont generate legacy messages
        rawInputDevices[0].hwndTarget = m_gameWindow.GetWindowHandle();
        // Mouse
        rawInputDevices[1].usUsagePage = 0x01;
        rawInputDevices[1].usUsage = 0x02; // Mouse
        rawInputDevices[1].dwFlags = RIDEV_NOLEGACY; // Dont generate legacy messages
        rawInputDevices[1].hwndTarget = m_gameWindow.GetWindowHandle();

        // Register devices
        // if(RegisterRawInputDevices(rawInputDevices, 2, sizeof(RAWINPUTDEVICE)) == false)
        // {
        //     cerr << "Failed to register input devices" << endl;
        // }

        g_NetworkManager.Initialize();

        // Controller Manager
        ControllerManager controllerManager;

        // Resource Manager
        // ResourceManager resourceManager;
        // string resourceDescPath = "resources/Models/ModelResources.xml";
        // resourceManager.LoadResourceDesc(resourceDescPath);

        // Load Global Resources
        // resourceManager.LoadScope(std::string("Global"));

        // Do Evnet Listener Registration Setup
        g_EventManager.AddListener(&g_InputHandler, EventType("keyboard_input_event"));
        g_EventManager.AddListener(&g_InputHandler, EventType("controller_connection_event"));
        g_EventManager.AddListener(&g_InputHandler, EventType("button_event"));
        g_EventManager.AddListener(&g_InputHandler, EventType("thumbstick_event"));

        g_EventManager.AddListener(&controllerManager, EventType("controller_vibration_event"));

        ECSManager entityManager;

        // Timing code
        const double dt = 1000.0 / 60.0 / 1000.0;

        // Memory code
        const u64 numGameBytes = 1024*1024*1024;
        void* memBlock = malloc(numGameBytes);

        LinearAllocator engineAllocator(numGameBytes, memBlock);

        auto* pStart = engineAllocator.Allocate(1024*1024*1024/2, 0);
        LinearAllocator frameAllocator(1024*1024*1024/2, pStart);

        EngineExport engineExport;
        engineExport.version = 1;
        engineExport.pECSManager = &entityManager;

#ifdef GAME_SHARED
        // Load the game code from Game.dll
        // cout << "Loading Game.dll..." << endl;

        // console->info("Loading Game Dll");

        HINSTANCE gameLibrary = LoadLibrary("Game.dll");
        if (!gameLibrary)
        {
            MessageBox(0, "Failed to load game dll", "Error", MB_OK);
            exit(1);
        }
        GetGameAPIPtr GetGameAPI;
        GetGameAPI = (GetGameAPIPtr)GetProcAddress(gameLibrary, "GetGameAPI");
        auto* pGameExportedStuff = GetGameAPI(engineAllocator, engineExport);
        IGame* pGame = pGameExportedStuff->game;

        ASSERT(pGame);
#else
        auto* pGameExportedStuff = GetGameAPI(engineAllocator, engineExport);
        IGame* pGame = pGameExportedStuff->game;
        ASSERT(pGame);
#endif
        pGame->Initialize();

        // Read in Scene
        Json::Value sceneRoot;
        std::fstream sceneConfig("configs/EngineConfig.json");
        sceneConfig >> sceneRoot;

        // Create entities
        auto entities = sceneRoot["scene"]["entities"];
        for (auto entity : entities)
        {
            Entity newEntity = entityManager.CreateEntity();
            FARLOR_LOG_INFO( "Creating Entity with ID: {}", newEntity );

            auto transformComponentValue = entity["TransformComponent"];
            if (!transformComponentValue.isNull())
            {
                TransformComponent component;
                component.m_entity = newEntity;
                component.m_position.x = transformComponentValue["position"]["x"].asFloat();
                component.m_position.y = transformComponentValue["position"]["y"].asFloat();
                component.m_position.z = transformComponentValue["position"]["z"].asFloat();

                component.m_rotation.x = DEGREE_TO_RAD(transformComponentValue["rotation"]["x"].asFloat());
                component.m_rotation.y = DEGREE_TO_RAD(transformComponentValue["rotation"]["y"].asFloat());
                component.m_rotation.z = DEGREE_TO_RAD(transformComponentValue["rotation"]["z"].asFloat());

                component.m_scale.x = transformComponentValue["scale"]["x"].asFloat();
                component.m_scale.y = transformComponentValue["scale"]["y"].asFloat();
                component.m_scale.z = transformComponentValue["scale"]["z"].asFloat();
                g_TransformManager.AddComponent(component);
                FARLOR_LOG_INFO("Loaded transform component")
            }

            auto physicsComponentValue = entity["PhysicsComponent"];
            if (!physicsComponentValue.isNull())
            {
                PhysicsComponent component;
                component.m_entity = newEntity;
                component.m_mass = physicsComponentValue["mass"].asFloat();

                g_PhysicsSystem.AddComponent(component);
                FARLOR_LOG_INFO("Loaded physics component")
            }

            auto renderingComponentValue = entity["RenderingComponent"];
            if (!renderingComponentValue.isNull())
            {
                RenderingComponent component;
                component.m_entity = newEntity;
                component.m_mesh = HashedString(renderingComponentValue["mesh"].asString());

                g_RenderingSystem.AddComponent(component);
                FARLOR_LOG_INFO("Loaded rendering component")
            }

            auto lightComponentValue = entity["LightComponent"];
            if (!lightComponentValue.isNull())
            {
                LightComponent component;
                component.m_entity = newEntity;

                component.m_direction.x = lightComponentValue["direction"]["x"].asFloat();
                component.m_direction.y = lightComponentValue["direction"]["y"].asFloat();
                component.m_direction.z = lightComponentValue["direction"]["z"].asFloat();

                component.m_color.x = lightComponentValue["color"]["r"].asFloat();
                component.m_color.y = lightComponentValue["color"]["g"].asFloat();
                component.m_color.z = lightComponentValue["color"]["b"].asFloat();

                component.m_lightRange.y = lightComponentValue["light-range"].asFloat();
                component.m_lightRange.x = lightComponentValue["light-range"].asFloat();
                component.m_lightRange.z = lightComponentValue["light-range"].asFloat();

                g_RenderingSystem.AddComponent(component);
                FARLOR_LOG_INFO("Loaded lighting component")
            }
        }

        Json::Value renderConfigRoot;
        std::fstream renderConfig("configs/RendererConfig.json");
        renderConfig >> renderConfigRoot;

        auto rendererGlobalResources = renderConfigRoot["global-resources"];
        auto rendererGlobalOptions = renderConfigRoot["options"];

        bool vsync = rendererGlobalOptions["vsync"].asBool();

        for (auto& resource : rendererGlobalResources)
        {
            string name = resource["name"].asString();
            string type = resource["type"].asString();
            int widthScale = resource["width-scale"].asInt();
            int heightScale = resource["height-scale"].asInt();
            string format = resource["format"].asString();

            g_RenderingSystem.RegisterGlobalResource(name, type, widthScale, heightScale, format);
        }

        g_RenderingSystem.CreateGlobalResources();

        // Load game shaders
        Json::Value shadersConfigRoot;
        std::fstream shaderConfig("configs/ShaderConfig.json");
        shaderConfig >> shadersConfigRoot;

        auto shaders = shadersConfigRoot["shaders"];
        for (auto& shader : shaders)
        {
            string filename = shader["filename"].asString();
            string shaderName = shader["name"].asString();

            bool hasVS = shader["hasVS"].asBool();
            bool hasPS = shader["hasPS"].asBool();

            vector<string> defines;

            auto defineValues = shader["defines"];
            for (auto& define : defineValues)
            {
                auto val = define.asString();
                defines.push_back(val);
            }

            g_RenderingSystem.RegisterShader(filename, shaderName, hasVS, hasPS, defines);
        }

        g_RenderingSystem.CreateShaders();


        // Load the game resources

        // Frame counting averages
        const int numFrameCount = 10;
        float frameTimes[numFrameCount];
        float runningTotal = 0.0f;
        int currentFrame = 0;

        // These are all the fixed updates
        FixedUpdate physicsUpdate{(float)dt};
        FixedUpdate controllerConnectionUpdate{2.0f};


        // Start controllers as connected if already when game starts
        controllerManager.PollConnection();


        g_TimerGame.Reset();

        // This is the game loop
        while (m_gameWindow.IsRunning())
        {
            // QueryPerformanceCounter(&counter);
            // double newTime = (double)counter.QuadPart / (double)frequency.QuadPart;
            // double frameTime = newTime - currentTime;
            g_TimerGame.Tick();

            float frameTime = g_TimerGame.DeltaTime();

            // cout << "Frame time: " << frameTime << endl;

            // Frame timing stuff
            frameTimes[currentFrame] = frameTime;
            runningTotal += frameTime;
            currentFrame++;

            if(currentFrame >= numFrameCount)
            {
                std::stringstream titleTextStream;
                titleTextStream << "Farlor Engine: " << (int)(1.0f / (runningTotal / numFrameCount));
                m_gameWindow.SetWindowTitleText(titleTextStream.str());
                currentFrame = 0;
                runningTotal = 0.0;
            }

            physicsUpdate.AccumulateTime(frameTime);
            controllerConnectionUpdate.AccumulateTime(frameTime);

            // Process messages
            m_gameWindow.ProcessSystemMessages();
            // Get polled input
            controllerManager.PollStates();
            // Process networking
            g_NetworkManager.Process();
            // Run through as many events as allowed in a tick
            g_EventManager.Tick();

            while (physicsUpdate.ShouldUpdate())
            {
                g_WaveParticles.Update(physicsUpdate.m_updateRate);
                // g_PhysicsSystem.Update(physicsUpdate.m_updateRate);
            }

            while(controllerConnectionUpdate.ShouldUpdate())
            {
                // FARLOR_LOG_INFO("Check controller connection")
                controllerManager.PollConnection();
            }

            g_MainCamera.Update(frameTime);
            g_WaveParticleCamera.Update(frameTime);

            g_RenderingSystem.Render();

            // Do the render step
            // We want the render to work on a "Engine/Physics State"
            frameAllocator.Clear();
        }

        engineAllocator.Clear();

        // Shuting down game
        FARLOR_LOG_INFO("Shutting down the game")
        pGame->Shutdown();

        FARLOR_LOG_INFO("Shutting down the engine system")

        // // Shutdown subsystems here
        m_gameWindow.Shutdown();

        return 0;
    }
}
