# basic bullshit
project(PowerBoot)
cmake_minimum_required(VERSION 3.28)
include(FetchContent)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FILE(GLOB_RECURSE COMMON_SOURCES ./src/*.c ./src/*.cpp ./src/*.r)
FILE(GLOB_RECURSE MAC_SOURCES ./mac/*.c ./mac/*.cpp ./mac/*.r)

# binary creation
if(CMAKE_SYSTEM_NAME MATCHES Retro)
add_application(${PROJECT_NAME} ${COMMON_SOURCES} ${MAC_SOURCES} CONSOLE)
else()
add_executable(${PROJECT_NAME} ${COMMON_SOURCES})
endif()

# cloning/linking libssh2.
# if we're compiling for mac, use the submodules.
if(CMAKE_SYSTEM_NAME MATCHES Retro)

# set up and build mbedtls
set(ENABLE_PROGRAMS OFF CACHE BOOL "disable mbedtls programs" FORCE)
set(UNSAFE_BUILD ON CACHE BOOL "ignore mac os jank for mbedtls" FORCE)
set(ENABLE_TESTING OFF CACHE BOOL "disable mbedtls tests" FORCE)
add_subdirectory(opentransport-mbedtls)

# set up and build libssh2
set(CRYPTO_BACKEND mbedTLS)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "disable libssh2 shared libs" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "disable libssh2 examples" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "disable libssh2 tests" FORCE)
add_subdirectory(opentransport-libssh2)

# otherwise, use FetchContent to get upstream libssh2.
else()
FetchContent_Declare(
    libssh2
    GIT_REPOSITORY "https://github.com/libssh2/libssh2/"
    GIT_PROGRESS TRUE
    BRANCH "libssh2-1.11.1"
)
FetchContent_MakeAvailable(libssh2)
endif()

# link libssh2.
# on mac, we also wanna link the other dependencies and add some function flags.
if(CMAKE_SYSTEM_NAME MATCHES Retro)
IF(CMAKE_SYSTEM_NAME MATCHES Retro68)
# for 68k
  set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-ffunction-sections -mcpu=68020 -O3 -Wall -Wextra -Wno-unused-parameter")
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-gc-sections -Wl,--mac-segments -Wl,${CMAKE_CURRENT_SOURCE_DIR}/ssheven.segmap")
  target_link_libraries(${PROJECT_NAME} libssh2 mbedtls mbedx509 mbedcrypto OpenTransportApp OpenTransport OpenTptInet retrocrt)
ELSE()
# for PPC
  set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-ffunction-sections -mcpu=601 -O3 -Wall -Wextra -Wno-unused-parameter")
  target_link_libraries(${PROJECT_NAME} libssh2 mbedtls mbedx509 mbedcrypto OpenTransportAppPPC OpenTransportLib OpenTptInternetLib retrocrt ThreadsLib)
ENDIF()
else()
target_link_libraries(${PROJECT_NAME} PUBLIC libssh2 mbedtls)
endif()

# misc
IF(CMAKE_SYSTEM_NAME MATCHES Retro)
include_directories("./mac/unixnet2mac/include")
target_compile_definitions(${PROJECT_NAME} PUBLIC "__RETRO__")
endif()