cmake_minimum_required(VERSION 3.10)
project(ctv VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find ncurses package
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Add executable
add_executable(ctv main.cpp)

#Link with required libraries
if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    target_link_libraries(ctv PRIVATE ${CURSES_LIBRARIES})
else()
    target_link_libraries(ctv PRIVATE stdc++fs ${CURSES_LIBRARIES})
endif()

# Install target
install(TARGETS ctv DESTINATION bin)
