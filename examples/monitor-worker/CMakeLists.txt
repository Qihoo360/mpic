
if (UNIX)
    file(GLOB SOURCES *.cc *.h ${PROJECT_SOURCE_DIR}/3rdparty/libdaemon-0.14/libdaemon/*.c)
    add_definitions(-D'LOCALSTATEDIR="./"')
    include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/3rdparty/libdaemon-0.14)
    set(LIBRARIES mpic_static glog)
    add_executable(examples_monitor_workers ${SOURCES})
    target_link_libraries(examples_monitor_workers ${LIBRARIES})
endif (UNIX)

