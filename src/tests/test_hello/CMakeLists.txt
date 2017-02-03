set(SOURCE_FILES
    test_hello.c)

add_executable(test_hello ${SOURCE_FILES})
target_link_libraries(test_hello samlib test_common)
