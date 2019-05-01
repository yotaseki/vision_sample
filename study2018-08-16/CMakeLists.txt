project(tutorial)

find_package(OpenCV)
add_executable(tutorial
    tutorial.cpp
)
target_link_libraries(tutorial
    ${OpenCV_LIBS}
)
add_executable(detect
    main.cpp
)
target_link_libraries(detect
    ${OpenCV_LIBS}
)
