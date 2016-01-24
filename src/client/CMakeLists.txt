project(shm_example)

find_package(Wayland 1.7.0 COMPONENTS Client Egl REQUIRED)
find_package(EGL REQUIRED)

include_directories(
    SYSTEM ${Wayland_INCLUDE_DIRS}
    ${DK_UTILS_INCLUDE_DIRS}
)

set(SRC
    waylandbuffer.cpp
    waylandclient.cpp
    waylandsurface.cpp
)

add_library(dk_wayland_client STATIC ${SRC})

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
    dk_utils
    dk_wayland_client
    ${Wayland_LIBRARIES}
)

set(PROJECT_NAME opengl_example)

add_executable(${PROJECT_NAME} test2.cpp)

add_definitions(-DWL_EGL_PLATFORM)

target_link_libraries(${PROJECT_NAME}
    ${Wayland_LIBRARIES}
    dk_utils
    ${EGL_LIBRARIES}
    GLESv2
    glapi
)

set(PROJECT_NAME qt_example)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)

include_directories(
    SYSTEM ${Qt5Widgets_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} main_qt.cpp)

# Use the Widgets module from Qt 5.
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
