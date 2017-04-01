file(GLOB SOURCES *.cc *.h)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/evpp)
set(LIBRARIES mpic_static glog evpp_static ${Boost_LIBRARIES})
if (UNIX)
    list(APPEND LIBRARIES dl)
endif (UNIX)

add_library(nfmpic_module_echo SHARED ${SOURCES})
target_link_libraries(nfmpic_module_echo ${LIBRARIES})
