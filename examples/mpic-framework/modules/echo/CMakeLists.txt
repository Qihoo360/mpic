file(GLOB SOURCES *.cc *.h)
include_directories(${PROJECT_SOURCE_DIR})
set(LIBRARIES mpic_static glog ${Boost_LIBRARIES})
if (UNIX)
    list(APPEND LIBRARIES dl)
endif (UNIX)

add_library(examples_module_echo SHARED ${SOURCES})
target_link_libraries(examples_module_echo ${LIBRARIES})
