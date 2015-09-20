project(dk_utils)
aux_source_directory(. SRC)

add_library(${PROJECT_NAME} STATIC ${SRC})
