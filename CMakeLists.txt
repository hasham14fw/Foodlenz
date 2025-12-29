cmake_minimum_required(VERSION 3.15)

project(RestaurantFinder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)

# Include directories
include_directories(include)
include_directories(src)
include_directories(${CMAKE_SOURCE_DIR}/asio/include)

# Source files
set(SOURCES
    src/main.cpp
    src/database/mongodb_client.cpp
    src/database/csv_database.cpp
    src/routes.cpp
)

# Create executable
add_executable(server ${SOURCES})

# Compile definitions
target_compile_definitions(server PRIVATE ASIO_STANDALONE _USE_MATH_DEFINES)
add_definitions(-DASIO_STANDALONE -D_USE_MATH_DEFINES)

# Link libraries
if(WIN32)
    target_link_libraries(server PRIVATE CURL::libcurl ws2_32 mswsock)
else()
    target_link_libraries(server PRIVATE CURL::libcurl pthread)
endif()

# Copy public directory to build directory
add_custom_command(TARGET server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/public
    ${CMAKE_BINARY_DIR}/public
)
