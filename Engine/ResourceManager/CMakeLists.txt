set (ENGINE_SOURCES

    ${ENGINE_SOURCES}

    ResourceManager/ResourceDesc.cpp
    ResourceManager/ResourceHandle.cpp
    ResourceManager/ResourceManager.cpp

    PARENT_SCOPE
)

set (ENGINE_INCLUDES

    ${ENGINE_INCLUDES}

    ResourceManager/ResourceDesc.h
    ResourceManager/ResourceHandle.h
    ResourceManager/ResourceManager.h
    ResourceManager/ResourceTypes.h

    PARENT_SCOPE
)
