cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(Balance VERSION 0.1 LANGUAGES CXX)

#include(ExternalProject)

#set(CMAKE_VERBOSE_MAKEFILE on)

# Add SFML
find_package(SFML 2 REQUIRED system window graphics audio)

if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIRS})
endif()

add_definitions(-D__GAME_VERSION=${game_VERSION})

# Enable C++11
set(CMAKE_CXX_FLAGS "-g --std=c++0x" ${CMAKE_CXX_FLAGS})

add_subdirectory(client)
