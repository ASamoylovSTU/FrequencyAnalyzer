find_package(Boost COMPONENTS chrono filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_library (Analyzer_lib Analyzer.cpp Analyzer.h)
add_executable (${PROJECT_NAME} main.cpp)
target_link_libraries (${PROJECT_NAME} Analyzer_lib ${Boost_LIBRARIES} )
