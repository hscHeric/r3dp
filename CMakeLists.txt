cmake_minimum_required(VERSION 3.15)
project(r3dp CXX)

find_package(Boost REQUIRED COMPONENTS graph)
find_package(CLI11 REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(r3dp src/r3dp.cpp src/main.cpp)

target_link_libraries(r3dp Boost::boost Boost::graph CLI11::CLI11
                      nlohmann_json::nlohmann_json)

install(
  TARGETS r3dp
  DESTINATION "."
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)
