cmake_minimum_required(VERSION 3.24)

project(OpenGitClient VERSION 0.0.2 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set(OGC_PORTABLE_DIR_NAME "OpenGitClient")
set(OGC_PACKAGE_ROOT "${CMAKE_BINARY_DIR}/package/${OGC_PORTABLE_DIR_NAME}")
set(OGC_DIST_DIR "${CMAKE_BINARY_DIR}/dist")
set(OGC_PORTABLE_ARCHIVE "${OGC_DIST_DIR}/${OGC_PORTABLE_DIR_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}.zip")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.4.2 REQUIRED COMPONENTS Widgets Test)

qt_standard_project_setup()

add_library(ogc_core STATIC
    src/app/application_bootstrap.cpp
    src/application/common/repository_session.cpp
    src/application/common/task_coordinator.cpp
    src/application/repository/repository_use_cases.cpp
    src/git-core/in_memory_git_backend.cpp
    src/platform/file_system_service.cpp
)

target_include_directories(ogc_core
    PUBLIC
        src
)

target_link_libraries(ogc_core
    PUBLIC
        Qt6::Core
)

qt_add_executable(open_git_client
    src/app/main.cpp
    src/ui/main_window/main_window.cpp
)

set_target_properties(open_git_client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(open_git_client
    PRIVATE
        ogc_core
        Qt6::Widgets
)

install(TARGETS open_git_client
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    BUNDLE DESTINATION "."
)

qt_generate_deploy_app_script(
    TARGET open_git_client
    OUTPUT_SCRIPT open_git_client_deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT "${open_git_client_deploy_script}")

add_custom_target(bundle_open_git_client
    COMMAND "${CMAKE_COMMAND}" -E rm -rf "${CMAKE_BINARY_DIR}/package"
    COMMAND "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}" --prefix "${OGC_PACKAGE_ROOT}"
    DEPENDS open_git_client
    USES_TERMINAL
    COMMENT "Bundling deployable Open Git Client runtime into ${OGC_PACKAGE_ROOT}/bin"
)

add_custom_target(package_open_git_client_portable
    COMMAND "${CMAKE_COMMAND}" -E rm -rf "${OGC_DIST_DIR}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${OGC_DIST_DIR}"
    COMMAND "${CMAKE_COMMAND}" -E tar "cf" "${OGC_PORTABLE_ARCHIVE}" --format=zip "${OGC_PORTABLE_DIR_NAME}"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/package"
    DEPENDS bundle_open_git_client
    USES_TERMINAL
    COMMENT "Archiving portable Open Git Client package to ${OGC_PORTABLE_ARCHIVE}"
)

enable_testing()

qt_add_executable(task_coordinator_tests
    tests/unit/task_coordinator_test.cpp
)

set_target_properties(task_coordinator_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
)

target_link_libraries(task_coordinator_tests
    PRIVATE
        ogc_core
        Qt6::Test
)

add_test(NAME task_coordinator_tests COMMAND task_coordinator_tests)

qt_add_executable(repository_use_cases_tests
    tests/unit/repository_use_cases_test.cpp
)

set_target_properties(repository_use_cases_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
)

target_link_libraries(repository_use_cases_tests
    PRIVATE
        ogc_core
        Qt6::Test
)

add_test(NAME repository_use_cases_tests COMMAND repository_use_cases_tests)
