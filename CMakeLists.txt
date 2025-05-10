cmake_minimum_required(VERSION 3.10)
project(chatBot01)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(CURL REQUIRED)
find_package(nlohmann_json 3.11.2 REQUIRED)

# Add executable
add_executable(chatBot01
    main.cpp
    ChatBot.cpp
)

# Link libraries
target_link_libraries(chatBot01 PRIVATE
    CURL::libcurl
    nlohmann_json::nlohmann_json
)
