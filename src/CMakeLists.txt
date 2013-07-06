include_directories(.)

set (SOURCE_FILES
    hub.cpp
    hue.cpp
    )

set (HEADER_FILES
    hub.h
    hue.h
    )

add_library(hue_cpp ${SOURCE_FILES})
target_link_libraries(hue_cpp curl)

