cmake_minimum_required(VERSION 3.16)

project(vulkan-tutorial LANGUAGES C CXX)

# Add third party libraries
add_subdirectory(third-party)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} glm volk glfw)