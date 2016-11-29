set (ENGINE_SOURCES

    ${ENGINE_SOURCES}

    Networking/NetworkManager.cpp
    Networking/Packet.cpp
    Networking/Socket.cpp

    PARENT_SCOPE
)

set (ENGINE_INCLUDES

    ${ENGINE_INCLUDES}

    Networking/NetworkManager.h
    Networking/Packet.h
    Networking/Socket.h

    PARENT_SCOPE
)
