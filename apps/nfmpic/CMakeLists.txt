
file(GLOB SOURCES *.cc *.h)
set(LIBRARIES mpic_static glog ${Boost_LIBRARIES})
if (UNIX)
    list(APPEND LIBRARIES dl)
endif (UNIX)

add_executable(nfmpic ${SOURCES})
target_link_libraries(nfmpic ${LIBRARIES})

