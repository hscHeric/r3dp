cmake_minimum_required(VERSION 3.15)
project(r3dp CXX)

find_package(Boost REQUIRED)


add_executable(r3dp src/r3dp.cpp src/main.cpp)

target_link_libraries(r3dp boost::boost)

install(TARGETS r3dp DESTINATION "."
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        )
