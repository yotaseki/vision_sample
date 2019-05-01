project(color_table)

find_package(OpenCV)
set(CMAKE_CXX_FLAGS "-std=c++1z ")
set(ROBOCUP_SRC_DIR $ENV{ROBOCUP_SRC_DIR})
include_directories(color_table
    ${ROBOCUP_SRC_DIR}/vision
)
add_executable(color_table
    main.cpp
    ${ROBOCUP_SRC_DIR}/vision/detector/color_table.cpp
    ${ROBOCUP_SRC_DIR}/vision/detector/color_table.h
    ${ROBOCUP_SRC_DIR}/vision/detector/convert_color.cpp
    ${ROBOCUP_SRC_DIR}/vision/detector/convert_color.h
)
target_link_libraries(color_table
    ${OpenCV_LIBS}
)
