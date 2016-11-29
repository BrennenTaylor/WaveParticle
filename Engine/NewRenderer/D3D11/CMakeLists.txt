set (ENGINE_SOURCES

    ${ENGINE_SOURCES}

    NewRenderer/D3D11/DXGIModeDesc.cpp
    NewRenderer/D3D11/DXGISwapChainDesc.cpp
    NewRenderer/D3D11/RasterizerStateDesc.cpp

    PARENT_SCOPE
)

set (ENGINE_INCLUDES

    ${ENGINE_INCLUDES}

    NewRenderer/D3D11/DXGIModeDesc.h
    NewRenderer/D3D11/DXGISwapChainDesc.h
    NewRenderer/D3D11/RasterizerStateDesc.h
    NewRenderer/D3D11/WrappedD3D11.h

    PARENT_SCOPE
)
