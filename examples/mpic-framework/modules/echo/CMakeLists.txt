
file(GLOB SOURCES *.cc *.h)
include_directories(${PROJECT_SOURCE_DIR})
set(LIBRARIES mpic_static glog boost_program_options-mt dl)
add_library(examples_module_echo SHARED ${SOURCES})
target_link_libraries(examples_module_echo ${LIBRARIES})
