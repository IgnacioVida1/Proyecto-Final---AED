cmake_minimum_required(VERSION 3.30)
project(Proyecto_AED)

set(CMAKE_CXX_STANDARD 20)

set(SDL2_DIR "${CMAKE_SOURCE_DIR}/Libs/x86_64-w64-mingw32_SDL2")
set(IMAGE_DIR "${CMAKE_SOURCE_DIR}/Libs/x86_64-w64-mingw32_SDL2_Image")


include_directories(${SDL2_DIR}/include)
include_directories(${SDL2_DIR}/include/SDL2)
include_directories(${IMAGE_DIR}/include)
include_directories(${IMAGE_DIR}/include/SDL2)

link_directories(${SDL2_DIR}/lib)
link_directories(${IMAGE_DIR}/lib)

add_executable(Proyecto_AED mainn.cpp
        Quad/Quad.cpp Quad/Quad.h
        Quad/Rect.cpp Quad/Rect.h
        Quad/Point.h
        Game/CollisionSystem.cpp Game/CollisionSystem.h
        Game/Game.cpp Game/Game.h
        Game/Constants.h
        Entity/Entity.h
        Entity/Entities.cpp Entity/Entities.h
        Renderer/Renderer.cpp Renderer/Renderer.h
        Renderer/Camera.h Renderer/Camera.cpp
)

target_link_libraries(Proyecto_AED
        mingw32
        SDL2main
        SDL2
        SDL2_ttf
        ${SDL2_DIR}/lib/libSDL2.dll.a
        ${IMAGE_DIR}/lib/libSDL2_image.dll.a
)

add_custom_command(TARGET Proyecto_AED POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DIR}/bin/SDL2.dll
        ${CMAKE_BINARY_DIR}/SDL2.dll
)

add_custom_command(TARGET Proyecto_AED POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DIR}/bin/SDL2_ttf.dll
        ${CMAKE_BINARY_DIR}/SDL2_ttf.dll
)

add_custom_command(TARGET Proyecto_AED POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${IMAGE_DIR}/bin/SDL2_image.dll
        ${CMAKE_BINARY_DIR}/
)