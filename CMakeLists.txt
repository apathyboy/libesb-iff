
cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(esb-iff VERSION 0.1 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
option(IFF_BUILD_TESTS "Build unit tests" ON)

set(PROJECT_NAME esb-iff)
set(IFF_INCLUDE_DESTINATION "include/esb")

# create iff library target
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	$<INSTALL_INTERFACE:${IFF_INCLUDE_DESTINATION}>)

# todo - remove this once the minimum cmake version supports enabling c++17 on MSVC
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_options(${PROJECT_NAME} INTERFACE "-std:c++latest")
endif()

# optionally build tests
if (IFF_BUILD_TESTS)
	add_executable(${PROJECT_NAME}_tests
		tests/iff_tests.cpp)

	target_link_libraries(${PROJECT_NAME}_tests ${PROJECT_NAME})

	set(AdditionalCatchParameters WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

	include(ParseAndAddCatchTests)
	ParseAndAddCatchTests(${PROJECT_NAME}_tests)
endif()

# install library
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME})
install(FILES "src/iff.hpp" DESTINATION ${IFF_INCLUDE_DESTINATION})
