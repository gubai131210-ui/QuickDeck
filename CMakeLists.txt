cmake_minimum_required(VERSION 3.19)
project(QuickDeck LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets Quick Qml Sql Test)

qt_standard_project_setup()

string(REPLACE "\\" "/" QUICKDECK_SOURCE_DIR "${CMAKE_SOURCE_DIR}")

set(QUICKDECK_CORE_SOURCES
    src/app/application.cpp
    src/app/application_context.cpp
    src/data/database_manager.cpp
    src/data/migration_runner.cpp
    src/data/repositories/sqlite_app_repository.cpp
    src/data/repositories/sqlite_clipboard_repository.cpp
    src/data/repositories/sqlite_settings_store.cpp
    src/platform/platform_factory.cpp
    src/platform/stub/stub_platform_services.cpp
    src/platform/win/win_hotkey_mapper.cpp
    src/platform/win/win_platform_services.cpp
    src/services/app_indexer.cpp
    src/services/clipboard_monitor.cpp
    src/services/logger.cpp
    src/services/search_ranking.cpp
    src/services/search_service.cpp
    src/ui/launcher_controller.cpp
    src/ui/models/app_search_model.cpp
    src/ui/models/clipboard_history_model.cpp
    src/ui/widgets/first_run_wizard.cpp
    src/ui/widgets/settings_window.cpp
    src/ui/widgets/tray_manager.cpp
)

set(QUICKDECK_CORE_HEADERS
    src/app/application.h
    src/app/application_context.h
    src/core/domain/types.h
    src/core/interfaces/i_platform_services.h
    src/core/interfaces/repositories.h
    src/core/result.h
    src/data/database_manager.h
    src/data/migration_runner.h
    src/data/repositories/sqlite_app_repository.h
    src/data/repositories/sqlite_clipboard_repository.h
    src/data/repositories/sqlite_settings_store.h
    src/platform/platform_factory.h
    src/platform/stub/stub_platform_services.h
    src/platform/win/win_hotkey_mapper.h
    src/platform/win/win_platform_services.h
    src/services/app_indexer.h
    src/services/clipboard_monitor.h
    src/services/logger.h
    src/services/search_ranking.h
    src/services/search_service.h
    src/ui/launcher_controller.h
    src/ui/models/app_search_model.h
    src/ui/models/clipboard_history_model.h
    src/ui/widgets/first_run_wizard.h
    src/ui/widgets/settings_window.h
    src/ui/widgets/tray_manager.h
)

add_library(QuickDeckCore STATIC
    ${QUICKDECK_CORE_SOURCES}
    ${QUICKDECK_CORE_HEADERS}
)

target_include_directories(QuickDeckCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(QuickDeckCore PRIVATE "QUICKDECK_SOURCE_DIR=\"${QUICKDECK_SOURCE_DIR}\"")

target_link_libraries(QuickDeckCore
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Sql
        Qt6::Widgets
        Qt6::Quick
        Qt6::Qml
)

qt_add_executable(QuickDeck
    WIN32 MACOSX_BUNDLE
    src/app/main.cpp
    resources/resources.qrc
)

qt_add_qml_module(QuickDeck
    URI QuickDeckLauncher
    VERSION 1.0
    QML_FILES
        src/ui/qml/LauncherOverlay.qml
    NO_PLUGIN
)

target_link_libraries(QuickDeck
    PRIVATE
        QuickDeckCore
)

add_custom_command(TARGET QuickDeck POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/migrations
        $<TARGET_FILE_DIR:QuickDeck>/migrations
    COMMENT "Copying database migrations"
)

install(TARGETS QuickDeck
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET QuickDeck
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

enable_testing()
add_subdirectory(tests)
