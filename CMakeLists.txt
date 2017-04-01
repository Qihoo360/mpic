cmake_minimum_required (VERSION 2.8)

project (safe-mpic C CXX)

if (UNIX)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release")
    endif()

    set (CXX_FLAGS
         -g
         -fPIC
         -Wall
         -Wno-unused-parameter
         -Wno-unused-function
         -Wunused-variable
         -Wunused-value
         -Wshadow
         -Wcast-qual
         -Wcast-align
         -Wwrite-strings
         -Wsign-compare
         -Winvalid-pch
         -fms-extensions
         -Wfloat-equal
         -Wextra
         -std=c++11
    )
    include_directories (${PROJECT_SOURCE_DIR}
                         ${PROJECT_SOURCE_DIR}/3rdparty)
else (UNIX)
set (CXX_FLAGS -DWIN32 -D_WIN32 -EHsc)
    include_directories(${PROJECT_SOURCE_DIR}
                        ${PROJECT_SOURCE_DIR}/3rdparty/glog-0.3.4/src/windows
                        ${PROJECT_SOURCE_DIR}/3rdparty/wininclude)
endif (UNIX)

string (REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

if (UNIX)
    SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb -D_DEBUG -DGOOGLE_STRIP_LOG=0")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -DGOOGLE_STRIP_LOG=1")
endif (UNIX)

if (CMAKE_BENCHMARK_TESTING)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DH_BENCHMARK_TESTING=1")
endif (CMAKE_BENCHMARK_TESTING)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

#set (MACOSX_RPATH ${LIBRARY_OUTPUT_PATH})
set (CMAKE_MACOSX_RPATH 0)

#set(BOOST_ROOT "d:/cppworkspace/boost_1_58_0/output")
find_package(Boost REQUIRED COMPONENTS program_options)
if (Boost_FOUND) 
    message(STATUS "Find boost")
    message(STATUS "Boost_INCLUDE_DIR=${Boost_INCLUDE_DIR}")
    message(STATUS "Boost_LIBRARY_DIR=${Boost_LIBRARY_DIR}")
    message(STATUS "Boost_FILESYSTEM_LIBRARY=${Boost_FILESYSTEM_LIBRARY}")
    message(STATUS "Boost_SYSTEM_LIBRARY=${Boost_SYSTEM_LIBRARY}")
    message(STATUS "Boost_CHRONO_LIBRARY=${Boost_CHRONO_LIBRARY}")
    message(STATUS "Boost_UNIT_TEST_FRAMEWORK_LIBRARY=${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}")
    message(STATUS "Boost_SIGNALS_LIBRARY=${Boost_SIGNALS_LIBRARY}")
    message(STATUS "Boost_THREAD_LIBRARY=${Boost_THREAD_LIBRARY}")
    message(STATUS "Boost_LIBRARY=${Boost_LIBRARY}")
    message(STATUS "Boost_LIBRARIES=${Boost_LIBRARIES}")
    include_directories(${Boost_INCLUDE_DIR})
    link_directories(${Boost_LIBRARY_DIRS})
else (Boost_FOUND)
    message (FATAL_ERROR "Cannot find boost")
endif (Boost_FOUND)

include_directories(${Boost_INCLUDE_DIR})

message (STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
message (STATUS "This is SOURCE dir " ${PROJECT_SOURCE_DIR})
message (STATUS "CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS})
message (STATUS "EXECUTABLE_OUTPUT_PATH " ${EXECUTABLE_OUTPUT_PATH})
message (STATUS "LIBRARY_OUTPUT_PATH " ${LIBRARY_OUTPUT_PATH})

add_subdirectory (mpic)
add_subdirectory (apps)
add_subdirectory (examples)

set (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include (utils)
include (packages)

include (CPack)
include (CTest)
