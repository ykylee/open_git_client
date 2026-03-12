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

find_package(wxWidgets REQUIRED COMPONENTS core base)
include("${wxWidgets_USE_FILE}")

set(OGC_FAIL_ON_UNRESOLVED_RUNTIME_DEPENDENCIES ON CACHE BOOL
    "Fail the build when runtime DLL discovery misses a non-system dependency."
)

macro(ogc_append_runtime_dependency_dir dir_path)
    if(dir_path AND IS_DIRECTORY "${dir_path}")
        list(APPEND OGC_RUNTIME_DEPENDENCY_DIRS "${dir_path}")
    endif()
endmacro()

set(OGC_RUNTIME_DEPENDENCY_DIRS "")
foreach(ogc_wx_library_dir IN LISTS wxWidgets_LIBRARY_DIRS)
    ogc_append_runtime_dependency_dir("${ogc_wx_library_dir}")
    get_filename_component(ogc_wx_library_parent_dir "${ogc_wx_library_dir}" DIRECTORY)
    ogc_append_runtime_dependency_dir("${ogc_wx_library_parent_dir}/bin")
endforeach()

if(DEFINED wxWidgets_ROOT_DIR)
    ogc_append_runtime_dependency_dir("${wxWidgets_ROOT_DIR}/bin")
endif()

foreach(ogc_wx_library IN LISTS wxWidgets_LIBRARIES)
    if(IS_ABSOLUTE "${ogc_wx_library}")
        get_filename_component(ogc_wx_library_dir "${ogc_wx_library}" DIRECTORY)
        ogc_append_runtime_dependency_dir("${ogc_wx_library_dir}")
        get_filename_component(ogc_wx_library_parent_dir "${ogc_wx_library_dir}" DIRECTORY)
        ogc_append_runtime_dependency_dir("${ogc_wx_library_parent_dir}/bin")
    endif()
endforeach()

get_filename_component(OGC_CXX_COMPILER_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
ogc_append_runtime_dependency_dir("${OGC_CXX_COMPILER_DIR}")
list(REMOVE_DUPLICATES OGC_RUNTIME_DEPENDENCY_DIRS)

set(OGC_COPY_RUNTIME_DEPENDENCIES_SCRIPT
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/CopyRuntimeDependencies.cmake"
)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CopyRuntimeDependencies.cmake.in"
    "${OGC_COPY_RUNTIME_DEPENDENCIES_SCRIPT}"
    @ONLY
)

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

add_executable(open_git_client
    src/app/main.cpp
    src/ui/commit_graph/commit_graph_view.cpp
    src/ui/main_window/main_window.cpp
)

set_target_properties(open_git_client PROPERTIES
    OUTPUT_NAME "OpenGitClient"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(open_git_client
    PRIVATE
        ogc_core
        ${wxWidgets_LIBRARIES}
)

if(WIN32)
    add_custom_command(TARGET open_git_client POST_BUILD
        COMMAND "${CMAKE_COMMAND}"
            -DINPUT_FILE="$<TARGET_FILE:open_git_client>"
            -DOUTPUT_DIR="$<TARGET_FILE_DIR:open_git_client>"
            -P "${OGC_COPY_RUNTIME_DEPENDENCIES_SCRIPT}"
        COMMENT "Copying runtime DLLs beside OpenGitClient.exe"
        VERBATIM
    )
endif()

install(TARGETS open_git_client
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    BUNDLE DESTINATION "."
)

if(WIN32)
    install(TARGETS open_git_client
        RUNTIME_DEPENDENCY_SET ogc_runtime_dependencies
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )

    install(RUNTIME_DEPENDENCY_SET ogc_runtime_dependencies
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
        PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
        POST_EXCLUDE_REGEXES ".*[Ss]ystem32/.*"
    )
endif()

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

add_executable(task_coordinator_tests
    tests/unit/task_coordinator_test.cpp
)

set_target_properties(task_coordinator_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
)

target_link_libraries(task_coordinator_tests
    PRIVATE
        ogc_core
)

add_test(NAME task_coordinator_tests COMMAND task_coordinator_tests)

add_executable(repository_use_cases_tests
    tests/unit/repository_use_cases_test.cpp
)

set_target_properties(repository_use_cases_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
)

target_link_libraries(repository_use_cases_tests
    PRIVATE
        ogc_core
)

add_test(NAME repository_use_cases_tests COMMAND repository_use_cases_tests)
