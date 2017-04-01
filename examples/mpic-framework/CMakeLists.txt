
file(GLOB SOURCES *.cc *.h)
set(LIBRARIES mpic_static glog ${Boost_LIBRARIES})
if (UNIX)
    list(APPEND LIBRARIES dl)
endif (UNIX)

add_executable(examples_mpic_framework ${SOURCES})
target_link_libraries(examples_mpic_framework ${LIBRARIES})

add_subdirectory (modules)
