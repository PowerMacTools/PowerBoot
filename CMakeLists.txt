project(PowerBoot)

cmake_minimum_required(VERSION 3.28)

FILE(GLOB_RECURSE sources ./src/*.c ./src/*.cpp)

# add_link_options(-lssh2 -lssl -lcrypto)

include(FetchContent)

FetchContent_Declare(
    libssh2
    GIT_REPOSITORY "https://github.com/libssh2/libssh2/"
    GIT_PROGRESS TRUE
    BRANCH "libssh2-1.11.1"
)
FetchContent_MakeAvailable(libssh2)

add_executable(${PROJECT_NAME} ${sources})

target_link_libraries(${PROJECT_NAME} PUBLIC libssh2)