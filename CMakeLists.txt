cmake_minimum_required(VERSION 3.10)
project(so-i-24-FedericaMayorga01)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add include directories
include_directories(/usr/local/include)

# Add library directories
link_directories(/usr/local/lib)

# Add your source files
file(GLOB SOURCES "src/*.c")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link against Prometheus libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    promhttp
    prom
)

message(STATUS "Linking with promhttp and prom libraries")
