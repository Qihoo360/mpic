
file(GLOB SOURCES *.cc *.h)
set(LIBRARIES mpic_static glog boost_program_options-mt dl)
add_executable(examples_mpic_framework ${SOURCES})
target_link_libraries(examples_mpic_framework ${LIBRARIES})


add_subdirectory (modules)
