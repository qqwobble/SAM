set(SOURCE_FILES
    main.c)

add_definitions(-DUSESDL)
add_executable(samtalk ${SOURCE_FILES})
target_link_libraries(samtalk PUBLIC samlib SDL)
