set (ENGINE_SOURCES

    ${ENGINE_SOURCES}

    Input/ControllerManager.cpp
    Input/InputHandler.cpp

    PARENT_SCOPE
)

set (ENGINE_INCLUDES

    ${ENGINE_INCLUDES}

    Input/Buttons.h
    Input/ControllerManager.h
    Input/InputHandler.h
    Input/Keys.h

    PARENT_SCOPE
)
