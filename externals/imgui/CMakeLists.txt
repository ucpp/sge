set(IMGUI_SOURCES 
    imgui_draw.cpp  
    imgui_impl_dx12.cpp  
    imgui_impl_win32.cpp  
    imgui_tables.cpp  
    imgui_widgets.cpp  
    imgui.cpp  
    imgui_freetype.cpp)

set(IMGUI_HEADERS 
    imconfig.h  
    imgui_impl_dx12.h  
    imgui_impl_win32.h  
    imgui_internal.h  
    imgui.h 
    imstb_rectpack.h 
    imstb_textedit.h 
    imstb_truetype.h  
    imgui_freetype.h)
    
set(THIRD_PARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../externals)
add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
target_include_directories(imgui PUBLIC
    ${THIRD_PARTY_PATH}/freetype/include/  
)
target_link_libraries(imgui PUBLIC freetype)