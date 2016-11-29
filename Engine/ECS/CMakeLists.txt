set (ENGINE_SOURCES

    ${ENGINE_SOURCES}

    ECS/Component.cpp
    ECS/EntityManager.cpp
    ECS/TransformManager.cpp

    PARENT_SCOPE
)

set (ENGINE_INCLUDES

    ${ENGINE_INCLUDES}

    ECS/Component.h
    ECS/EntityManager.h
    ECS/Entity.h
    ECS/TransformComponent.h
    ECS/TransformManager.h

    PARENT_SCOPE
)
