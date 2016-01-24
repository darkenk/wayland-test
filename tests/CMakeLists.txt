include(${CMAKE_CURRENT_SOURCE_DIR}/../external/dk_utils/cmake/ToolsGtest.txt)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -Wno-cast-qual"
)

find_package(Wayland 1.7.0 COMPONENTS Server Client REQUIRED)
set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/../src/")
set(INC "${Wayland_INCLUDE_DIRS};${SRC}")

set(SRC_LIST
    nullbackend.cpp
    testclient.cpp
    ${SRC}/server/backend/idisplaybackend.cpp
)
add_library(test_lib STATIC ${SRC_LIST})
target_include_directories(test_lib PUBLIC ${INC})
set(LINK "${Wayland_LIBRARIES};wrapper;dk_wayland_client;test_lib")

add_gtest(test1 "test1.cpp" "${INC}" "${LINK}")
