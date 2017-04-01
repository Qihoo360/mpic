
file(GLOB SOURCES *.cc *.h)
set(LIBRARIES mpic glog ${Boost_LIBRARIES})

add_executable(nfmpic ${SOURCES})
target_link_libraries(nfmpic ${LIBRARIES})

