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
    ECSManager* m_pECSManager = nullptr;
};

struct GameExport
{
    int version = 0;
    IGame* game = nullptr;
};

}

GAME_EXPORT Farlor::GameExport* GetGameAPI(Farlor::LinearAllocator& pAllocator, Farlor::EngineExport& engineExport);
