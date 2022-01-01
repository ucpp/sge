set(IMGUI_SOURCES 
    imgui.cpp 
    imgui_demo.cpp 
    imgui_draw.cpp 
    imgui_widgets.cpp 
    imgui_tables.cpp 
    imgui_impl_glfw.cpp 
    imgui_impl_opengl3.cpp)

set(IMGUI_HEADERS 
    imconfig.h 
    imgui.h 
    imgui_internal.h 
    imstb_rectpack.h 
    imstb_textedit.h 
    imstb_truetype.h 
    imgui_impl_glfw.h 
    imgui_impl_opengl3_loader.h 
    imgui_impl_opengl3.h)

add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
target_include_directories(imgui PUBLIC ../glfw/include/ ..)