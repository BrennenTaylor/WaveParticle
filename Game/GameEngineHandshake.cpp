#include "Game.h"

#include <Core/Memory/LinearAllocator.h>
#include <EngineExport.h>

using namespace Farlor;

// This is the handshake method
GAME_EXPORT GameExport* GetGameAPI(LinearAllocator& pAllocator, EngineExport& engineExport)
{
    /*Game* pGame = (Game*)pAllocator.Allocate(sizeof(Game));*/
    Game* pGame = (Game*)pAllocator.Allocate(sizeof(Game));
    pGame = new(pGame) Game(engineExport);
    ASSERT(pGame);

    // GameExport* pGameExport = new GameExport { 1, pGame };
    GameExport* pGameExport = AllocateNew<GameExport>(pAllocator);
    pGameExport->version = 1;
    pGameExport->game = pGame;
    ASSERT(pGameExport);

    return pGameExport;
}
