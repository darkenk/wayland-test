include(${CMAKE_CURRENT_SOURCE_DIR}/../external/dk_utils/cmake/ToolsGtest.txt)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -Wno-cast-qual"
)

find_package(Wayland 1.7.0 COMPONENTS Server Client REQUIRED)

set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/../src/")
set(INC "${Wayland_INCLUDE_DIRS};${SRC}")
set(LINK "${Wayland_LIBRARIES};wrapper")
set(SRC_LIST

)

add_gtest(test1 "test1.cpp;nullbackend.cpp;${SRC}/server/backend/idisplaybackend.cpp" "${INC}" "${LINK}")
