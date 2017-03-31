
file(GLOB mpic_SRCS *.cc */*.cc *.h */*.h ${PROJECT_SOURCE_DIR}/3rdparty/libdaemon-0.14/libdaemon/*.c)
file(GLOB mpic_PUBLIC_HEADERS *.h)

message(STATUS "mpic_SRCS : " ${mpic_SRCS})

set (MACOSX_RPATH ${LIBRARY_OUTPUT_PATH})

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/3rdparty/libdaemon-0.14)
add_definitions(-D'LOCALSTATEDIR="./"')

add_library(mpic_static STATIC ${mpic_SRCS})

if (UNIX)
    add_library(mpic SHARED ${mpic_SRCS})
    target_link_libraries(mpic        glog boost_program_options-mt)
    target_link_libraries(mpic_static glog boost_program_options-mt)

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
	link_directories(${PROJECT_SOURCE_DIR}/vsprojects/bin/${CMAKE_BUILD_TYPE})
    target_link_libraries(mpic_static libglog_static)
endif (UNIX)

