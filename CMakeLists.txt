cmake_minimum_required(VERSION 3.0)
project(paddle-mobile)

option(DEBUGING "enable debug mode" ON)
option(USE_OPENMP "openmp support" OFF)
option(USE_EXCEPTION "use std exception" ON)
option(LOG_PROFILE "log profile" ON)
# select the platform to build
option(CPU "armv7 with neon" ON)
option(MALI_GPU "mali gpu" OFF)
option(FPGA "fpga" OFF)
set(DEBUGING ON)

file(GLOB_RECURSE PADDLE_MOBILE_CC src/*.cc src/*.cpp src/*.c)
file(GLOB_RECURSE PADDLE_MOBILE_H src/*.h)

if (CPU)
  add_definitions(-DPADDLE_MOBILE_CPU)
else()
    list(REMOVE_ITEM PADDLE_MOBILE_CC ${CMAKE_CURRENT_SOURCE_DIR}/src/operators/kernel/arm/*.h)
    list(REMOVE_ITEM PADDLE_MOBILE_CC ${CMAKE_CURRENT_SOURCE_DIR}/src/operators/kernel/arm/*.cc)
    list(REMOVE_ITEM PADDLE_MOBILE_CC ${CMAKE_CURRENT_SOURCE_DIR}/src/operators/kernel/arm/*.cpp)

endif()

if (MALI_GPU)
    add_definitions(-DPADDLE_MOBILE_MALI_GPU)
    add_definitions(-DUSE_ACL=1)
    add_definitions(-DUSE_OPENCL)
    set(ACL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src/operators/kernel/mali/ACL_Android)
    include_directories(${ACL_ROOT} ${ACL_ROOT}/include)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${ACL_ROOT}/build")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -larm_compute")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -larm_compute_core")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -larm_compute_graph")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${ACL_ROOT}/build/opencl-1.2-stubs")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lOpenCL")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_ACL=1")
else()
    list(REMOVE_ITEM PADDLE_MOBILE_CC ${CMAKE_CURRENT_SOURCE_DIR}/src/operators/kernel/mali/*.h)
    list(REMOVE_ITEM PADDLE_MOBILE_CC ${CMAKE_CURRENT_SOURCE_DIR}/src/operators/kernel/mali/*.cc)
    list(REMOVE_ITEM PADDLE_MOBILE_CC ${CMAKE_CURRENT_SOURCE_DIR}/src/operators/kernel/mali/*.cpp)


endif()

if(FPGA)
    add_definitions(-DPADDLE_MOBILE_FPGA)
else()
    list(REMOVE_ITEM PADDLE_MOBILE_CC ${CMAKE_CURRENT_SOURCE_DIR}/src/operators/kernel/fpga/*.h)
    list(REMOVE_ITEM PADDLE_MOBILE_CC ${CMAKE_CURRENT_SOURCE_DIR}/src/operators/kernel/fpga/*.cc)
    list(REMOVE_ITEM PADDLE_MOBILE_CC ${CMAKE_CURRENT_SOURCE_DIR}/src/operators/kernel/fpga/*.cpp)
endif()


set(CMAKE_CXX_FLAGS "-std=c++14 -O3 -s ${CMAKE_CXX_FLAGS}")
if (DEBUGING)
  message(STATUS "debug")
  set(CMAKE_BUILD_TYPE Debug)
  set(CMAKE_CXX_FLAGS_DEBUG "-g -DNDEBUG")
  add_definitions(-DPADDLE_MOBILE_DEBUG)
  if (ANDROID_NDK_TOOLCHAIN_INCLUDED)
    add_definitions(-DARMV7)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -llog")
  endif ()
else ()
  set(CMAKE_BUILD_TYPE Release)
  set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG")
  add_definitions(-fvisibility=hidden -fvisibility-inlines-hidden)
endif ()

if (USE_EXCEPTION)
    message(STATUS "use exception")
    add_definitions(-DENABLE_EXCEPTION)
    add_definitions(-fexceptions)
else()
    add_definitions(-fno-exceptions)
endif ()

if (LOG_PROFILE)
    add_definitions(-DPADDLE_MOBILE_PROFILE)
endif()

if(USE_OPENMP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
    add_definitions(-DPADDLE_MOBILE_USE_OPENMP)
endif()




if (NOT ANDROID_NDK_TOOLCHAIN_INCLUDED)
  list(REMOVE_ITEM PADDLE_MOBILE_CC ${CMAKE_CURRENT_SOURCE_DIR}/src/jni/*.cpp)
  list(REMOVE_ITEM PADDLE_MOBILE_CC ${CMAKE_CURRENT_SOURCE_DIR}/src/jni/*.h)
  list(REMOVE_ITEM PADDLE_MOBILE_CC ${CMAKE_CURRENT_SOURCE_DIR}/src/operators/math/math_func_neon.h)
endif ()

include_directories(src/)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)

include("${CMAKE_CURRENT_LIST_DIR}/tools/op.cmake")

# if (IS_IOS)
#     add_library(paddle-mobile STATIC ${PADDLE_MOBILE_CC} ${PADDLE_MOBILE_H})
if (ANDROID_NDK_TOOLCHAIN_INCLUDED)
    list(REMOVE_DUPLICATES CMAKE_CXX_FLAGS)
    add_library(paddle-mobile SHARED ${PADDLE_MOBILE_CC} ${PADDLE_MOBILE_H})
else ()
    add_library(paddle-mobile SHARED ${PADDLE_MOBILE_CC} ${PADDLE_MOBILE_H})
endif ()

if(DEBUGING)
    add_subdirectory(test)
endif()


