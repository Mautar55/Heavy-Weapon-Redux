cmake_minimum_required(VERSION 4.1)
project(attempt_0 C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

add_executable(attempt_0 src/main.c src/wutils.h src/wutils.c
        src/random_shapes.h
        src/random_shapes.c
        src/game_character.h
        src/game_character.c
        src/generated_assets.h
        src/generated_assets.c
        src/tank_bullet_atlas.h
)
add_executable(test src/test.c src/wutils.h src/wutils.c)
add_executable(asset_gen src/asset_gen.c src/wutils.h src/wutils.c
        src/game_globals.h)

include(FetchContent)

FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
)

# Optional: don't build raylib examples/games
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

# find_package(raylib CONFIG REQUIRED)
target_link_libraries(asset_gen PRIVATE raylib)
target_link_libraries(test PRIVATE raylib)
target_link_libraries(attempt_0 PRIVATE raylib)
