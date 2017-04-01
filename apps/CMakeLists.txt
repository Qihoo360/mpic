#include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/3rdparty)

set(LIBRARIES mpic_static glog ${Boost_LIBRARIES})
if (UNIX)
else(UNIX)
link_directories(${PROJECT_SOURCE_DIR}/build/lib/${CMAKE_BUILD_TYPE}/
				 ${PROJECT_SOURCE_DIR}/3rdparty/glog-0.3.4/${CMAKE_BUILD_TYPE})
endif(UNIX)

add_subdirectory (nfmpic)
add_subdirectory (modules)
