#pragma once

#include "Export.h"

#include "ECS/EntityManager.h"

#include "Core/Memory/LinearAllocator.h"
#include "EngineExport.h"

namespace Farlor {

class IGame
{
public:
    virtual void Initialize() = 0;
    virtual void Update(float dt) = 0;

    virtual void Shutdown() = 0;

protected:
    ECSManager* m_pECSManager;
};

struct GameExport
{
    int version;
    IGame* game;
};

}

GAME_EXPORT Farlor::GameExport* GetGameAPI(Farlor::LinearAllocator& pAllocator, Farlor::EngineExport& engineExport);
