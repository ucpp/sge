cmake_minimum_required(VERSION 3.10)
project(DirectXTex)

set(LIBRARY_HEADERS
    DirectXTex/DirectXTex.h
    DirectXTex/DirectXTex.inl
)

set(LIBRARY_SOURCES
    DirectXTex/BC.h
    DirectXTex/DDS.h
    DirectXTex/DirectXTexP.h
    DirectXTex/filters.h
    DirectXTex/scoped.h
    DirectXTex/BC.cpp
    DirectXTex/BC4BC5.cpp
    DirectXTex/BC6HBC7.cpp
    DirectXTex/DirectXTexCompress.cpp
    DirectXTex/DirectXTexConvert.cpp
    DirectXTex/DirectXTexDDS.cpp
    DirectXTex/DirectXTexHDR.cpp
    DirectXTex/DirectXTexImage.cpp
    DirectXTex/DirectXTexMipmaps.cpp
    DirectXTex/DirectXTexMisc.cpp
    DirectXTex/DirectXTexNormalMaps.cpp
    DirectXTex/DirectXTexPMAlpha.cpp
    DirectXTex/DirectXTexResize.cpp
    DirectXTex/DirectXTexTGA.cpp
    DirectXTex/DirectXTexUtil.cpp
)

if(WIN32)
   list(APPEND LIBRARY_SOURCES
       DirectXTex/DirectXTexFlipRotate.cpp
       DirectXTex/DirectXTexWIC.cpp
   )
endif()

add_library(DirectXTex STATIC ${LIBRARY_HEADERS} ${LIBRARY_SOURCES})

target_include_directories(DirectXTex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_link_libraries(DirectXTex d3d12 dxguid)
endif()
