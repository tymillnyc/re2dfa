cmake_minimum_required(VERSION 3.17)
project(newpr)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_STANDARD 17)

link_directories(newpr ${PROJECT_SOURCE_DIR}/)
add_executable(newpr main.cpp)
set_target_properties(newpr PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "YES")

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
    target_link_libraries(newpr RegexCheckerCore_Darwin)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(newpr RegexCheckerCore_Linux)
elseif(CMAKE_HOST_WIN32)
    target_link_libraries(newpr RegexCheckerCore_Windows)
endif()
install(TARGETS newpr DESTINATION .)