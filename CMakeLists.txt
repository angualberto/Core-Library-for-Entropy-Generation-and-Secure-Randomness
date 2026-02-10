cmake_minimum_required(VERSION 3.16)

project(agle VERSION 2.0.0 LANGUAGES C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(AGLE_BUILD_EXAMPLES "Build example programs" ON)
option(AGLE_BUILD_TESTS "Build test programs" ON)

find_package(OpenSSL REQUIRED)

add_library(agle src/agle.c)

set_target_properties(agle PROPERTIES
    OUTPUT_NAME agle
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_compile_features(agle PUBLIC c_std_99)

target_compile_options(agle PRIVATE
    $<$<C_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)

target_include_directories(agle
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(agle PUBLIC OpenSSL::Crypto)

install(TARGETS agle
    EXPORT agleTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES include/agle.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT agleTargets
    FILE agleTargets.cmake
    NAMESPACE AGLE::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/agle
)

configure_package_config_file(
    cmake/AGLEConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/AGLEConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/agle
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/AGLEConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/AGLEConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/AGLEConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/agle
)

configure_file(
    agle.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/agle.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/agle.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

if(AGLE_BUILD_EXAMPLES)
    add_executable(agle_examples examples/agle_examples.c)
    target_link_libraries(agle_examples PRIVATE agle)

    add_executable(agle_quickstart examples/agle_quickstart.c)
    target_link_libraries(agle_quickstart PRIVATE agle)

    add_executable(example_api_tokens examples/example_api_tokens.c)
    target_link_libraries(example_api_tokens PRIVATE agle)

    add_executable(example_auth_system examples/example_auth_system.c)
    target_link_libraries(example_auth_system PRIVATE agle)

    add_executable(example_password_gen examples/example_password_gen.c)
    target_link_libraries(example_password_gen PRIVATE agle)
endif()

if(AGLE_BUILD_TESTS)
    enable_testing()

    add_executable(test_shake256 tests/test_shake256.c)
    target_link_libraries(test_shake256 PRIVATE OpenSSL::Crypto)

    add_test(NAME test_shake256 COMMAND test_shake256)
endif()
