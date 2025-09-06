set(TEST_TARGET "sdl_test")

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "*.cpp")
add_executable(${TEST_TARGET} ${SRC})
target_include_directories(${TEST_TARGET}
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(${TEST_TARGET}
    PRIVATE sdl
)
