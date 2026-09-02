cmake_minimum_required(VERSION 3.19)

find_package(Qt6 REQUIRED COMPONENTS Test)

string(REPLACE "\\" "/" QUICKDECK_SOURCE_DIR "${CMAKE_SOURCE_DIR}")

function(quickdeck_add_test name source)
    qt_add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE QuickDeckCore Qt6::Test)
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_compile_definitions(${name} PRIVATE "QUICKDECK_SOURCE_DIR=\"${QUICKDECK_SOURCE_DIR}\"")
    add_test(NAME ${name} COMMAND ${name})
    set_tests_properties(${name} PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
    add_custom_command(TARGET ${name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/migrations
            $<TARGET_FILE_DIR:${name}>/migrations
        COMMENT "Copy migrations for ${name}"
    )
endfunction()

quickdeck_add_test(SearchRankingTest search_ranking_test.cpp)
quickdeck_add_test(PathResolverTest path_resolver_test.cpp)
quickdeck_add_test(ClipboardDeduplicationTest clipboard_deduplication_test.cpp)
quickdeck_add_test(WinHotkeyMapperTest win_hotkey_mapper_test.cpp)
quickdeck_add_test(LauncherControllerTest launcher_controller_test.cpp)
quickdeck_add_test(MigrationRunnerTest migration_runner_test.cpp)
