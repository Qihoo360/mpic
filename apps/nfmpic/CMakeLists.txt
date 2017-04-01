
file(GLOB SOURCES *.cc *.h)
if (UNIX)
    set(LIBRARIES mpic glog ${Boost_LIBRARIES})
else (UNIX)
    set(LIBRARIES mpic_static glog ${Boost_LIBRARIES})
endif (UNIX)

add_executable(nfmpic ${SOURCES})
target_link_libraries(nfmpic ${LIBRARIES})

