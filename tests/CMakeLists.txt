include_directories(../src)

set (SOURCE_FILES
    main.cpp
    hue_addr_tests.cpp
    )

add_executable(tests ${SOURCE_FILES})
target_link_libraries(tests gtest pthread hue_cpp)

