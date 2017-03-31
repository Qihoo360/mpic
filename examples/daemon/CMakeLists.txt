
if (UNIX)
    file(GLOB SOURCES *.cc *.h ${PROJECT_SOURCE_DIR}/3rdparty/libdaemon-0.14/libdaemon/*.c)
    include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/3rdparty/libdaemon-0.14)
    add_definitions(-D'LOCALSTATEDIR="./"')
    add_executable(examples_daemon ${SOURCES})
endif (UNIX)

