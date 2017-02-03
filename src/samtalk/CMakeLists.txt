set(SOURCE_FILES
    main.c)

find_package(SDL)

if (SDL_FOUND)
    add_definitions(-DUSESDL)
    add_definitions(-D_SDL_main_h)
endif()

add_executable(samtalk ${SOURCE_FILES})

if (SDL_FOUND)
    include_directories(${SDL_INCLUDE_DIR})
    target_link_libraries(samtalk samlib ${SDL_LIBRARY})
else()
    target_link_libraries(samtalk samlib)
endif()
