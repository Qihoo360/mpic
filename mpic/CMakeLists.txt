
file(GLOB mpic_SRCS *.cc */*.cc *.h */*.h)
file(GLOB libdaemon_SRCS ${PROJECT_SOURCE_DIR}/3rdparty/libdaemon-0.14/libdaemon/*.c)
file(GLOB mpic_PUBLIC_HEADERS *.h)

message(STATUS "mpic_SRCS : " ${mpic_SRCS})

set (MACOSX_RPATH ${LIBRARY_OUTPUT_PATH})

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/3rdparty/libdaemon-0.14)
add_definitions(-D'LOCALSTATEDIR="./"')

if (UNIX)
    list(APPEND mpic_SRCS ${libdaemon_SRCS})
    message(STATUS "mpic_SRCS : " ${mpic_SRCS})
    add_library(mpic_static STATIC ${mpic_SRCS})
    add_library(mpic SHARED ${mpic_SRCS})
    target_link_libraries(mpic        glog dl ${Boost_LIBRARIES})
    target_link_libraries(mpic_static glog dl ${Boost_LIBRARIES})

    set (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
    include (utils)
    include (packages)
    set_target_properties (
        mpic PROPERTIES
        VERSION     "${PACKAGE_VERSION}"
        SOVERSION   "${PACKAGE_SOVERSION}"
    )

    install (
      TARGETS mpic mpic_static
      EXPORT ${PACKAGE_NAME}
      RUNTIME DESTINATION bin
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib)

    install (FILES ${mpic_PUBLIC_HEADERS} DESTINATION "include/mpic")

else (UNIX)
    add_library(mpic_static STATIC ${mpic_SRCS})
	link_directories(${PROJECT_SOURCE_DIR}/vsprojects/bin/${CMAKE_BUILD_TYPE})
    target_link_libraries(mpic_static glog)
endif (UNIX)

