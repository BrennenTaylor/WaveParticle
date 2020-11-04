#include "Game.h"

namespace Farlor
{
    Game::Game(EngineExport& engineExport)
    {
        m_pECSManager = engineExport.pECSManager;
    }

    void Game::Initialize()
    {
    }

    void Game::Update(float dt)
    {
    }

    void Game::Shutdown()
    {
    }
}
