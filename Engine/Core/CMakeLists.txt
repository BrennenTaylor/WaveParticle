set (ENGINE_SOURCES

    ${ENGINE_SOURCES}

    Core/Events/ButtonEvent.cpp
    Core/Events/ControllerConnectionEvent.cpp
    Core/Events/ControllerVibrationEvent.cpp
    Core/Events/EventManager.cpp
    Core/Events/KeyboardEvent.cpp
    Core/Events/ThumbstickEvent.cpp

    Core/Memory/BaseAllocator.cpp
    Core/Memory/LinearAllocator.cpp
    Core/Memory/PoolAllocator.cpp
    Core/Memory/ProxyAllocator.cpp
    Core/Memory/StackAllocator.cpp

    Core/Engine.cpp
    Core/FixedUpdate.cpp
    Core/Timer.cpp
    Core/Win32Window.cpp

    PARENT_SCOPE
)

set (ENGINE_INCLUDES

    ${ENGINE_INCLUDES}

    Core/Events/ButtonEvent.h
    Core/Events/ControllerConnectionEvent.h
    Core/Events/ControllerVibrationEvent.h
    Core/Events/EventData.h
    Core/Events/EventListener.h
    Core/Events/EventManager.h
    Core/Events/KeyboardEvent.h
    Core/Events/ThumbstickEvent.h

    Core/Memory/BaseAllocator.h
    Core/Memory/LinearAllocator.h
    Core/Memory/MemoryAlignmentUtils.h
    Core/Memory/PoolAllocator.h
    Core/Memory/ProxyAllocator.h
    Core/Memory/StackAllocator.h

    Core/Engine.h
    Core/FixedUpdate.h
    Core/EngineConfig.h
    Core/Timer.h
    Core/Win32Window.h
    Core/Window.h

    

    PARENT_SCOPE
)
