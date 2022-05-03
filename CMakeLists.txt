cmake_minimum_required(VERSION 3.15)
set(ver_major 2)
set(ver_minor 0)
set(ver_patch 0)
set(version ${ver_major}.${ver_minor}.${ver_patch})
set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_SOURCE_DIR}/configs.cmake)
project(kmarkley-arr
  VERSION ${version}
  LANGUAGES CXX
)
include(CTest)
if(BUILD_TESTING)
  if (NOT TARGET tests)
    add_custom_target(tests)
  endif()
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

#
# Configurations
#
get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
set(build_types normal profile debug)
if(is_multi_config)
  foreach(type ${build_types})
    if(NOT ${type} IN_LIST CMAKE_CONFIGURATION_TYPES)
      list(APPEND CMAKE_CONFIGURATION_TYPES ${type})
    endif()
  endforeach()
else()
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY
    STRINGS "${build_types}"
  )
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE normal CACHE STRING "" FORCE)
  elseif(NOT CMAKE_BUILD_TYPE IN_LIST build_types)
    message("Build type should be one of: ${build_types}")
    message(FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")
  endif()
endif()
message(STATUS "Build type is: ${CMAKE_BUILD_TYPE}")
include(CMakeInitializeConfigs)
cmake_initialize_per_config_variable(CMAKE_CXX_FLAGS           "Flags used by the compiler")
cmake_initialize_per_config_variable(CMAKE_EXE_LINKER_FLAGS    "Flags used by the linker")
cmake_initialize_per_config_variable(CMAKE_SHARED_LINKER_FLAGS "Flags used by the linker")
cmake_initialize_per_config_variable(CMAKE_STATIC_LINKER_FLAGS "Flags used by the linker")
cmake_initialize_per_config_variable(CMAKE_MODULE_LINKER_FLAGS "Flags used by the linker")

#
# Warnings
#
if     (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR
        ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  add_compile_options(
    -Wall -Wpedantic -Wextra -Wswitch -Wconversion
    -Wsign-compare -Wsign-conversion
    -Wredundant-decls -Wshadow
    -Wfloat-equal -Wnull-dereference -Wcast-qual
  )
endif()
if     (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  add_compile_options(
    -Warith-conversion -Wlogical-op
    -Wduplicated-branches -Wduplicated-cond
    -Wcast-align=strict
    -Wno-tautological-compare
  )
endif()

#
# Test definition
#
function(define_simple_test name main lib)
  if(BUILD_TESTING)
    add_executable(${name} EXCLUDE_FROM_ALL ${main})
    target_link_libraries(${name} PRIVATE ${lib})
    add_test(NAME ${name} COMMAND ${name})
    add_dependencies(tests ${name})
    set_tests_properties(${name} PROPERTIES LABELS ${lib})
  endif()
endfunction()

#
# arr
#
add_library(arr)
set_target_properties(arr PROPERTIES
  VERSION ${version}
  SOVERSION ${ver_major}
)
target_include_directories(arr
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
# PUBLIC path-to-opaque
)
target_sources(arr
  PUBLIC

# buffers
arr/acq_rel_accumulator.hpp
arr/recent_accumulator.hpp
arr/buffer_base.hpp
arr/buffer_direction.hpp
arr/buffer_transfer.hpp
arr/fifo.hpp

# utilities
arr/special_member.hpp
arr/basic_ptr.hpp
arr/mask.hpp
arr/swap_macros.hpp
arr/clone_macros.hpp
arr/copy_ptr.hpp
arr/algorithm.hpp

# scope utilities
arr/finally.hpp
arr/restore.hpp
arr/rollback.hpp

# exceptions
arr/source_context.hpp
arr/context_exception.hpp
arr/errno_exception.hpp
arr/syscall_exception.hpp
arr/path_exception.hpp

# system function wrappers
arr/cstdlib.hpp
arr/dirent.hpp
arr/fcntl.hpp
arr/glob.hpp
arr/unistd.hpp
arr/wait.hpp

# system resources
arr/directory.hpp
arr/file_descriptor.hpp
arr/pipe.hpp
arr/process_id.hpp
arr/temp_template.hpp
arr/temp_file.hpp
arr/temp_dir.hpp

# directory traversal
arr/directory_sequence.hpp
arr/recursive_directory_sequence.hpp

# child processes
arr/arg_env.hpp
arr/fd_stream.hpp
arr/filter_stream.hpp
arr/child.hpp
arr/execute.hpp

arr/arr.hpp

  PRIVATE
arr/source_context.cpp
arr/context_exception.cpp
arr/errno_exception.cpp
arr/syscall_exception.cpp
arr/path_exception.cpp
arr/cstdlib.cpp
arr/dirent.cpp
arr/fcntl.cpp
arr/glob.cpp
arr/unistd.cpp
arr/wait.cpp
arr/directory.cpp
arr/file_descriptor.cpp
arr/process_id.cpp
arr/temp_file.cpp
arr/temp_dir.cpp
arr/directory_sequence.cpp
arr/recursive_directory_sequence.cpp
arr/arg_env.cpp
arr/filter_stream.cpp
arr/child.cpp
arr/execute.cpp
arr/tracing_base.test.hpp
arr/report.test.hpp
)
set(arr_tests
arr/acq_rel_accumulator.test.cpp
arr/recent_accumulator.test.cpp
arr/buffer_base.test.cpp
arr/buffer_direction.test.cpp
arr/buffer_transfer.test.cpp
arr/fifo.test.cpp
arr/fifo_concurrency.test.cpp
arr/basic_ptr.test.cpp
arr/mask.test.cpp
arr/swap_macros.test.cpp
arr/clone_macros.test.cpp
arr/copy_ptr.test.cpp
arr/algorithm.test.cpp
arr/finally.test.cpp
arr/restore.test.cpp
arr/rollback.test.cpp
arr/source_context.test.cpp
arr/context_exception.test.cpp
arr/errno_exception.test.cpp
arr/glob.test.cpp
arr/directory.test.cpp
arr/file_descriptor.test.cpp
arr/process_id.test.cpp
arr/temp_file.test.cpp
arr/temp_dir.test.cpp
arr/recursive_directory_sequence.test.cpp
arr/arg_env.test.cpp
arr/filter_stream.test.cpp
arr/child.test.cpp
arr/execute.test.cpp
)
foreach(item ${arr_tests})
  get_filename_component(name ${item} NAME_WE)
  define_simple_test(${name} ${item} arr)
endforeach()

find_package(Threads REQUIRED)
target_link_libraries(fifo_concurrency PRIVATE Threads::Threads)

target_compile_options(swap_macros PRIVATE -Wno-self-move -Wno-self-assign-overloaded)
target_compile_options(context_exception PRIVATE -Wno-self-move -Wno-self-assign-overloaded)
target_compile_options(errno_exception PRIVATE -Wno-self-move)
target_compile_options(fifo_concurrency PRIVATE -Wno-sign-conversion)
