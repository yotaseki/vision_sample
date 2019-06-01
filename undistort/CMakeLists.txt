project(undistort)

set(CMAKE_CXX_FLAGS "-std=c++1z ")
find_package(OpenCV REQUIRED)

add_executable(undistort
    main.cpp
    undistort.cpp
    undistort.h
)

target_link_libraries(undistort
    ${OpenCV_LIBS}
)

