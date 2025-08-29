cmake_minimum_required(VERSION 3.15)
project(r3dp CXX)

find_package(Boost REQUIRED)
find_package(CLI11 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenMP)

if(OpenMP_CXX_FOUND)
  message(STATUS "OpenMP encontrado: ${OpenMP_CXX_VERSION}")
else()
  message(
    WARNING
      "OpenMP não encontrado. A avaliação paralela rodará sem paralelismo.")
endif()

set(CORE_SOURCES src/core/graph.cpp)

set(GA_SOURCES
    src/genetic_algorithm/population.cpp src/genetic_algorithm/r3d_decoder.cpp
    src/genetic_algorithm/pop_generator.cpp)

set(BRKGA_SOURCES src/brkga/population.cpp)

set(MAIN_SOURCES src/r3dp.cpp src/main.cpp)

set(SOURCES ${MAIN_SOURCES} ${CORE_SOURCES} ${GA_SOURCES} ${BRKGA_SOURCES})

add_executable(r3dp ${SOURCES})

target_link_libraries(r3dp PUBLIC Boost::boost CLI11::CLI11
                                  nlohmann_json::nlohmann_json)

target_compile_options(
  r3dp PRIVATE $<$<BOOL:${OpenMP_CXX_FOUND}>:${OpenMP_CXX_FLAGS}>)

target_link_libraries(
  r3dp PRIVATE $<$<BOOL:${OpenMP_CXX_FOUND}>:${OpenMP_CXX_LIBRARIES}>)

install(
  TARGETS r3dp
  DESTINATION "."
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)
