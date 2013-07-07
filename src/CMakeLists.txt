include_directories(.)

set (SOURCE_FILES
    hub.cpp
    hue.cpp
    light.cpp
    )

set (HEADER_FILES
    hub.h
    hue.h
    light.h
    )

set (DEMO_SOURCE_FILES
    demo/demo.cpp
    )

add_library(hue_cpp ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(hue_cpp curl jsoncpp)

add_executable(demo ${DEMO_SOURCE_FILES})
target_link_libraries(demo hue_cpp curl)

