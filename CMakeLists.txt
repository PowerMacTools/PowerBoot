project(PowerBoot)

cmake_minimum_required(VERSION 3.28)
include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FILE(GLOB_RECURSE sources ./src/*.c ./src/*.cpp ./src/*.r)

# binary creation
if(CMAKE_SYSTEM_NAME MATCHES Retro)
add_application(${PROJECT_NAME} ${sources} CONSOLE)
else()
add_executable(${PROJECT_NAME} ${sources})
endif()

# libssh2 settings
set(CRYPTO_BACKEND mbedTLS)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "disable libssh2 shared libs" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "disable libssh2 examples" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "disable libssh2 tests" FORCE)

# cloning/linking libssh2
if(CMAKE_SYSTEM_NAME MATCHES Retro)
# the libssh2 port depends on another mbedtls port
set(ENABLE_PROGRAMS OFF CACHE BOOL "disable mbedtls programs" FORCE)
set(UNSAFE_BUILD ON CACHE BOOL "ignore mac os jank for mbedtls" FORCE)
set(ENABLE_TESTING OFF CACHE BOOL "disable mbedtls tests" FORCE)
add_subdirectory(opentransport-mbedtls)

add_subdirectory(opentransport-libssh2)

else()
FetchContent_Declare(
    libssh2
    GIT_REPOSITORY "https://github.com/libssh2/libssh2/"
    GIT_PROGRESS TRUE
    BRANCH "libssh2-1.11.1"
)
FetchContent_MakeAvailable(libssh2)
endif()


if(CMAKE_SYSTEM_NAME MATCHES Retro)
IF(CMAKE_SYSTEM_NAME MATCHES Retro68)
# for 68k
  set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-ffunction-sections -mcpu=68020 -O3 -Wall -Wextra -Wno-unused-parameter")
  target_link_libraries(${PROJECT_NAME} libssh2 mbedtls mbedx509 mbedcrypto retrocrt OpenTptATalk OpenTptClientCompat11 OpenTptInet OpenTptKernelUtils OpenTptMiscUtils OpenTptModule OpenTptUtils OpenTransport OpenTransportApp OpenTransportExtn)
ELSE()
# for PPC
  set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-ffunction-sections -mcpu=601 -O3 -Wall -Wextra -Wno-unused-parameter")
  target_link_libraries(${PROJECT_NAME} libssh2 mbedtls mbedx509 mbedcrypto OpenTransportAppPPC OpenTransportLib OpenTptInternetLib retrocrt ThreadsLib)
ENDIF()
else()
target_link_libraries(${PROJECT_NAME} PUBLIC libssh2 mbedtls)
endif()