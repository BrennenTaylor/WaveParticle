set (ENGINE_SOURCES

    ${ENGINE_SOURCES}

    Physics/PhysicsComponent.cpp
    Physics/PhysicsSystem.cpp
    Physics/ParticleSystem.cpp
    Physics/LineSegment.cpp

    Physics/Particle.cpp
    Physics/ParticleSystemTimer.cpp

    PARENT_SCOPE
)

set (ENGINE_INCLUDES

    ${ENGINE_INCLUDES}

    Physics/PhysicsComponent.h
    Physics/PhysicsSystem.h
    Physics/Particle.h
    Physics/ParticleSystem.h
    Physics/Plane.h

    Physics/ParticleSystemTimer.h

    PARENT_SCOPE
)
