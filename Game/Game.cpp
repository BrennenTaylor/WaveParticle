#include "Game.h"

#include <iostream>

using namespace std;

namespace Farlor
{
    Game::Game(EngineExport& engineExport)
    {
        m_pECSManager = engineExport.pECSManager;
    }

    void Game::Initialize()
    {
        // Entity leftPaddle = m_pECSManager->CreateEntity();
        // Entity rightPaddle = m_pECSManager->CreateEntity();
        // Entity ball = m_pECSManager->CreateEntity();
    }

    void Game::Update(float dt)
    {
    }

    void Game::Shutdown()
    {
    }
}
