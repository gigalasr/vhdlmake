cmake_minimum_required(VERSION 3.15)
project(vhdlmake)
set(CMAKE_CXX_STANDARD 20)

if(CMAKE_BUILD_TYPE STREQUAL "Debug") 	
    add_definitions(-DDEBUG) 
endif()

set(SOURCES 
    "src/Main.cpp"
    "src/DependencyGraph.cpp"
    "src/Builder.cpp"
    "src/Unit.cpp"
)
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(vendor/json)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

add_library(${PROJECT_NAME}_lib ${SOURCES})
target_link_libraries(${PROJECT_NAME}_lib PRIVATE nlohmann_json::nlohmann_json)

add_subdirectory(vendor/googletest)
add_subdirectory(tests)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
