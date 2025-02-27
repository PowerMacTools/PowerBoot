project(PowerBoot)

cmake_minimum_required(VERSION 3.28)

FILE(GLOB_RECURSE sources ./src/*.c ./src/*.cpp)

add_link_options(-lssh2 -lssl -lcrypto)

add_executable(${PROJECT_NAME} ${sources})