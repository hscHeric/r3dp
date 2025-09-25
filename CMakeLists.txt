cmake_minimum_required(VERSION 4.1.1)

project(
  r3dp
  VERSION 0.1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

option(R3DP_BUILD_TESTS "Build tests" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
  include("${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
endif()

find_package(Boost REQUIRED)
find_package(CLI11 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenMP)

if(OpenMP_FOUND)
  message(STATUS "OpenMP encontrado: ${OpenMP_CXX_VERSION}")
else()
  message(
    WARNING
      "OpenMP não encontrado. A avaliação paralela rodará sem paralelismo.")
endif()

# ============================
# #CONFIGURACAO LIBS (apenas dependências externas)
# ============================
add_library(r3dp_libs INTERFACE)
add_library(r3dp::libs ALIAS r3dp_libs)

target_link_libraries(r3dp_libs INTERFACE boost::boost CLI11::CLI11
                                          nlohmann_json::nlohmann_json)

if(OpenMP_FOUND)
  target_link_libraries(r3dp_libs INTERFACE OpenMP::OpenMP_CXX)
endif()

# ============================
# #CONFIGURACAO CORE (libs externas + código de core/*)
# ============================
add_library(r3dp_core STATIC src/core/graph.cpp # adicione outros .cpp do core
)
add_library(r3dp::core ALIAS r3dp_core)

target_include_directories(r3dp_core PUBLIC ${CMAKE_SOURCE_DIR}/src/core/)

target_link_libraries(r3dp_core PUBLIC r3dp::libs)

# ============================
# #CONFIGURACAO BRKGA (libs externas + core/* + meta/brkga/*)
# ============================
add_library(
  r3dp_brkga STATIC src/meta/brkga/population.cpp # adicione outros .cpp do
                                                  # BRKGA
)
add_library(r3dp::brkga ALIAS r3dp_brkga)

target_include_directories(r3dp_brkga
PUBLIC ${CMAKE_SOURCE_DIR}/src/meta/brkga/)

target_link_libraries(r3dp_brkga PUBLIC r3dp::core r3dp::libs)

# ============================
# AGREGADOR PARA EXEMPLOS (só para exemplos, não obrigatório)
# ============================
add_library(r3dp_all INTERFACE)
add_library(r3dp::all ALIAS r3dp_all)

target_link_libraries(r3dp_all INTERFACE r3dp::libs r3dp::core r3dp::brkga)

# ============================
# EXECUTÁVEIS
# ============================
# Executável que depende apenas do BRKGA
add_executable(brkga_main src/brkga_main.cpp)
target_link_libraries(brkga_main PRIVATE r3dp::brkga)

# Exemplo que usa tudo
add_executable(rng_example examples/rng_example.cpp)
target_link_libraries(rng_example PRIVATE r3dp::all)
