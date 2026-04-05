cmake_minimum_required(VERSION 3.20)
project(TilkyEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)

add_executable(TilkyEngine
        main.cpp
        config.h
        Headers/InputManager.h
        src/InputManager.cpp
        Headers/Vector.h
        src/Player.cpp
        Headers/Player.h
        Headers/GameTime.h
)

target_link_libraries(TilkyEngine PRIVATE
        SDL3::SDL3
        $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>
        SDL3_ttf::SDL3_ttf
)