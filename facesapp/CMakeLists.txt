add_executable(facesapp main.cpp)

set_target_properties(facesapp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)

target_include_directories(facesapp PRIVATE
    ${JSONCPP_INCLUDE_DIR}
)

target_link_libraries(facesapp PRIVATE
    ${JSONCPP_LIBRARY}
)


