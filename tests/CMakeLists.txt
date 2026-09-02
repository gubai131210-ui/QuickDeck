cmake_minimum_required(VERSION 3.19)

find_package(Qt6 REQUIRED COMPONENTS Test)

function(quickdeck_add_test name source)
    qt_add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE QuickDeckCore Qt6::Test)
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_compile_definitions(${name} PRIVATE QUICKDECK_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
    add_test(NAME ${name} COMMAND ${name})
endfunction()

quickdeck_add_test(SearchRankingTest search_ranking_test.cpp)
quickdeck_add_test(PathResolverTest path_resolver_test.cpp)
quickdeck_add_test(ClipboardDeduplicationTest clipboard_deduplication_test.cpp)
