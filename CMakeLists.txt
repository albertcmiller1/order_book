#####################################
# Define Project-Wide Settings
#####################################
cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

# Define the project name and language
project(order_book
	LANGUAGES CXX
)

# Make sure Findasio.cmake module is found
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Check if Crow is the main project
# set(CROW_IS_MAIN_PROJECT OFF)
# if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
#   set(CROW_IS_MAIN_PROJECT ON)
# endif()

# Set required C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_BUILD_TYPE "Release") # "debug"


#####################################
# Define Options
#####################################
option(CROW_INSTALL            "Add install step for Crow"              ON )

# Possible values: ssl, compression
# option(CROW_FEATURES					 "Enable features extending Crow's abilities" "")

#####################################
# Define Targets
#####################################
add_library(Crow INTERFACE)
add_library(Crow::Crow ALIAS Crow)

target_include_directories(Crow
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

# find_package(asio REQUIRED)

# target_link_libraries(Crow
# 	INTERFACE
# 		asio::asio
# )

target_compile_definitions(Crow INTERFACE "")

#####################################
# Install Files
#####################################
if(CROW_INSTALL)
	install(TARGETS Crow EXPORT CrowTargets)
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)

	install(EXPORT CrowTargets
		FILE CrowTargets.cmake
		NAMESPACE Crow::
		DESTINATION lib/cmake/Crow
	)

	include(CMakePackageConfigHelpers)
	configure_package_config_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/CrowConfig.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/CrowConfig.cmake"
		INSTALL_DESTINATION lib/cmake/Crow
	)
	install(FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/Findasio.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CrowConfig.cmake"
    DESTINATION lib/cmake/Crow
  )
endif()

include(CPack)

add_executable(socket socket.cpp order_book.cpp)
target_link_libraries(socket PUBLIC Crow::Crow)

#####################################
# Uninstall Files
#####################################
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
