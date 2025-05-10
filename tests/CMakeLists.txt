# Тестовый исполняемый файл
add_executable(sync_tests
    MarkerThreadTests.cpp
)

# Настройки для тестов
target_include_directories(sync_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(sync_tests PRIVATE
    sync_utils
    gtest
    gtest_main
)

# Регистрация тестов
include(GoogleTest)
gtest_discover_tests(sync_tests)

add_test(NAME sync_tests COMMAND sync_tests)