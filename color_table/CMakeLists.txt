project(color_table)
find_package(OpenCV)

set(CMAKE_CXX_FLAGS "-std=c++1z ")
add_executable(color_table
    main.cpp
    detector/color_table.cpp
    detector/color_table.h
    detector/convert_color.cpp
    detector/convert_color.h
)
target_link_libraries(color_table
    ${OpenCV_LIBS}
)
