set(SOURCE_FILES
    test_phonemes.c)

add_executable(test_phonemes ${SOURCE_FILES})
target_link_libraries(test_phonemes samlib test_common)
