cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(hcsTests LANGUAGES CXX)

set(TEST_DIR "test/")
set(BINARY_DIR "bin/")

# ---- Options ----

option(ENABLE_TEST_COVERAGE "Enable test coverage" OFF)
option(TEST_INSTALLED_VERSION "Test the version found by find_package" OFF)

# --- Import tools ----

include(../cmake/tools.cmake)

# ---- Dependencies ----

include(../cmake/CPM.cmake)
CPMAddPackage("gh:TheLartians/Format.cmake@1.7.0")

if(TEST_INSTALLED_VERSION)
  find_package(hcs REQUIRED)
else()
  CPMAddPackage(NAME hcs SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
endif()

# ---- Create binary ----

file(GLOB test_srcs CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
foreach(f ${test_srcs})
  get_filename_component(test_prog ${f} NAME_WE)
  add_executable(${test_prog} "${test_prog}.cpp")
  target_link_libraries(${test_prog} hcs gmp gmpxx)
  add_test(${test_prog} "${BINARY_DIR}/${test_prog}")
endforeach()

# ---- Add hcsTests ----

enable_testing()
