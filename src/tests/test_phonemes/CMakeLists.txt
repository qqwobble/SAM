set(SOURCE_FILES
    test_phonemes.c)

add_executable(test_reciter ${SOURCE_FILES})
target_link_libraries(test_reciter samlib test_common)
