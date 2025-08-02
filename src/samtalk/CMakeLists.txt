set(SOURCE_FILES
    main.c)

#find_package(SDL)
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-static)

if (SDL3_FOUND)
    add_definitions(-DUSESDL)
    add_definitions(-D_SDL_main_h)
else()
    message( FATAL_ERROR "Yeah, you want SDL for this." )
endif()

add_executable(samtalk ${SOURCE_FILES})

if (SDL3_FOUND)
    target_link_libraries(samtalk samlib SDL3::SDL3-static)
else()
    target_link_libraries(samtalk samlib)
endif()
