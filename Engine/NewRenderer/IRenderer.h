#pragma once

namespace Farlor
{
    class IRenderer
    {
    public:

        Renderer(GameWindow* gameWindow);

        virtual bool Initialize(bool vsync, float screenNear, float screenDepth) = 0;
        virtual bool Render() = 0;

        virtual void SetBackgroundColor(float* backgroundColor) = 0;

        virtual void ApplyPipelineState() = 0;

        virtual void ClearBuffers() = 0;

        virtual void Reset() = 0;

    protected:

    }
}
