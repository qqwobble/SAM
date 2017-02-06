# add test directory as define
add_definitions("-DSAM_TEST_DIR=\"${CMAKE_CURRENT_LIST_DIR}\"")

add_subdirectory(test_common)
add_subdirectory(test_hello)
add_subdirectory(test_phonemes)
