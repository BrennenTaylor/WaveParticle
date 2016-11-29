#pragma once

#include <IGame.h>
#include <EngineExport.h>

namespace Farlor {

class Game : public IGame
{
public:
    Game(EngineExport& engineExport);

    virtual void Initialize() override;
    virtual void Update(float dt) override;
    virtual void Shutdown() override;
};

}
