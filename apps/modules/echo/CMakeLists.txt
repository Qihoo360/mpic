file(GLOB SOURCES *.cc *.h)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/evpp)
link_directories(${PROJECT_SOURCE_DIR}/3rdparty/evpp/build-release/lib/)
set(LIBRARIES mpic glog evpp ${Boost_LIBRARIES})

add_library(nfmpic_module_echo SHARED ${SOURCES})
target_link_libraries(nfmpic_module_echo ${LIBRARIES})
