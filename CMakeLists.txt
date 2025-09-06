cmake_minimum_required(VERSION 3.16)

set(TARGET sdl)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(
    ${TARGET}
    VERSION 0.1.0
    DESCRIPTION "sdl"
    HOMEPAGE_URL "https://github.com/m1dsolo/sdl"
    LANGUAGES C CXX
)

set(SDL_ROOT ${PROJECT_SOURCE_DIR}/third_party/SDL)
set(SDL_IMAGE ${PROJECT_SOURCE_DIR}/third_party/SDL_image)
set(SDL_MIXER ${PROJECT_SOURCE_DIR}/third_party/SDL_mixer)
set(SDL_TTF ${PROJECT_SOURCE_DIR}/third_party/SDL_ttf)

add_subdirectory(${SDL_ROOT})
add_subdirectory(${SDL_IMAGE})
add_subdirectory(${SDL_MIXER})
add_subdirectory(${SDL_TTF})

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "src/*.cpp")
add_library(${TARGET} OBJECT ${SRC})
target_include_directories(${TARGET}
    PUBLIC include
    PUBLIC ${SDL_ROOT}/include
    PUBLIC ${SDL_IMAGE}/include 
    PUBLIC ${SDL_MIXER}/include 
    PUBLIC ${SDL_TTF}/include 
)
target_link_libraries(${TARGET}
    PUBLIC SDL3::SDL3
    PUBLIC SDL3_image::SDL3_image
    PUBLIC SDL3_mixer::SDL3_mixer
    PUBLIC SDL3_ttf::SDL3_ttf
)

# add_subdirectory(test)
