find_package(Boost COMPONENTS unit_test_framework filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories (${PROJECT_SOURCE_DIR}/src)

add_definitions (-DBOOST_TEST_DYN_LINK)

add_executable(Analyzer_test test.cpp)

target_link_libraries(Analyzer_test ${Boost_LIBRARIES} Analyzer_lib)

add_test(NAME Analyzer_test COMMAND Analyzer_test)
