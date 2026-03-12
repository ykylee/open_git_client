cmake_minimum_required(VERSION 3.24)

project(OpenGitClient VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

target_link_libraries(open_git_client
    PRIVATE
        ogc_core
        Qt6::Widgets
)

enable_testing()

qt_add_executable(task_coordinator_tests
    tests/unit/task_coordinator_test.cpp
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

target_link_libraries(repository_use_cases_tests
    PRIVATE
        ogc_core
        Qt6::Test
)

add_test(NAME repository_use_cases_tests COMMAND repository_use_cases_tests)
