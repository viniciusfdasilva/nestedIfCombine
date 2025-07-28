cmake_minimum_required(VERSION 3.13)
project(NestedIfCombine VERSION 0.1 LANGUAGES CXX)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(NestedIfCombine MODULE NestedIfCombine.cpp)

set_target_properties(NestedIfCombine PROPERTIES
  COMPILE_FLAGS "-fno-rtti"
)

llvm_map_components_to_libnames(llvm_libs support core irreader passes)
target_link_libraries(NestedIfCombine PRIVATE ${llvm_libs})

