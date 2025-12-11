
set(HEADERS
    include/facelib.h
)

set(SOURCES
    src/facelib.cpp
)

add_library(faceslib SHARED
    ${HEADERS}
    ${SOURCES}
)

target_include_directories(faceslib PRIVATE
    ${OpenCV_INCLUDE_DIR}
    ${JSONCPP_INCLUDE_DIR}
    include
)

set_target_properties(faceslib PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR}
)

target_link_libraries(faceslib PRIVATE
    ${OpenCV_LIBS}
    ${JSONCPP_LIBRARY}
)

install(TARGETS faceslib
    LIBRARY DESTINATION lib        # Linux/macOS
    RUNTIME DESTINATION bin        # Windows DLL
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/model
        DESTINATION model/face_detection_yunet_2023mar.onnx)

