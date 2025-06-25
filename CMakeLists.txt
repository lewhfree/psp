cmake_minimum_required(VERSION 3.11)

project(hello)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
include_directories(include)
add_executable(${PROJECT_NAME} main.c
    src/stlloader.c
    src/boilerplate.c
    src/trianglethings.c)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pspdebug
    pspdisplay
    pspge
    pspgu
    pspgum
    pspctrl
)

# Create an EBOOT.PBP file
create_pbp_file(
    TARGET ${PROJECT_NAME}
    ICON_PATH NULL
    BACKGROUND_PATH NULL
    PREVIEW_PATH NULL
    TITLE ${PROJECT_NAME}
    VERSION 01.00
)

