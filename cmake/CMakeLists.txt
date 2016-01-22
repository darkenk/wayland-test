# determine which compiler is used
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    # using Clang
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} \
        -std=c++11 \
        -D__cplusplus=201103L \
        -Werror -Weverything \
        -Wno-c++98-compat \
        -Wno-c++98-compat-pedantic \
        -Wno-exit-time-destructors \
        -Wno-implicit-fallthrough \
        -Wno-missing-braces \
        -Wno-padded \
        -Wno-gnu-zero-variadic-macro-arguments \
        -Wno-sign-conversion \
        -Wno-weak-vtables \
        -Wno-format-extra-args"
    )
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    # using GCC

    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} \
        -std=c++11 \
        -Werror \
        -Wall \
        -Wextra \
        -Wpedantic \
        -Wcast-align \
        -Wcast-qual \
        -Wdouble-promotion \
        -Wfloat-equal \
        -Wformat=2 \
        -Winit-self \
        -Winvalid-pch \
        -Wlogical-op \
        -Wmissing-declarations \
        -Wmissing-include-dirs \
        -Wnoexcept \
        -Wold-style-cast \
        -Woverloaded-virtual \
        -Wredundant-decls \
        -Wshadow \
        -Wsign-promo \
        -Wstrict-null-sentinel \
        -Wstrict-overflow=5 \
        -Wtrampolines \
        -Wundef \
        -Wunsafe-loop-optimizations \
        -Wvector-operation-performance \
        -Wno-sign-conversion \
        -Wno-format-extra-args"
    )
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")

endif()
