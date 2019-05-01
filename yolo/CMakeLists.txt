project(yolo)

find_package(OpenCV)
set(CMAKE_CXX_FLAGS "-std=c++1z ")
set(ROBOCUP_SRC_DIR $ENV{ROBOCUP_SRC_DIR})
set(DARKNET_DIR $ENV{DARKNET_DIR})

include_directories(
    ${ROBOCUP_SRC_DIR}/vision
    ${ROBOCUP_SRC_DIR}/HPL
    ${DARKNET_DIR}/include
)

link_directories(
    ${DARKNET_DIR}
)

add_executable(yolo
    main.cpp
    ${ROBOCUP_SRC_DIR}/vision/detector/detector_yolo.cpp
    ${ROBOCUP_SRC_DIR}/vision/detector/detector_yolo.h
    ${ROBOCUP_SRC_DIR}/vision/detector/color_table.cpp
    ${ROBOCUP_SRC_DIR}/vision/detector/color_table.h
    ${ROBOCUP_SRC_DIR}/vision/detector/object_detector.cpp
    ${ROBOCUP_SRC_DIR}/vision/detector/object_detector.h
    ${ROBOCUP_SRC_DIR}/vision/detector/detector_tool.cpp
    ${ROBOCUP_SRC_DIR}/vision/detector/detector_tool.h
    ${ROBOCUP_SRC_DIR}/vision/detector/convert_color.cpp
    ${ROBOCUP_SRC_DIR}/vision/detector/convert_color.h
)

target_link_libraries(yolo
    darknet
    ${OpenCV_LIBS}
)
