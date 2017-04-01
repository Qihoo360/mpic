#include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/3rdparty)

if (UNIX)
set(LIBRARIES mpic_static glog boost_program_options-mt)
else(UNIX)
set(LIBRARIES mpic_static libglog_static)
link_directories(${PROJECT_SOURCE_DIR}/vsprojects/bin/${CMAKE_BUILD_TYPE}/
				 ${PROJECT_SOURCE_DIR}/3rdparty/glog-0.3.4/${CMAKE_BUILD_TYPE})
endif(UNIX)

add_subdirectory (daemon)
add_subdirectory (master-workers)
add_subdirectory (monitor-worker)
#add_subdirectory (modules)
add_subdirectory (mpic-framework)
