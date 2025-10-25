cmake_minimum_required(VERSION 3.26)
project(GrA_SovereignOS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(GRA_ENABLE_SSH "Enable libssh2 features" ON)
option(GRA_STRICT "Enable strict warnings" ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Network Sql)

# Windows DPAPI (optional)
if (WIN32)
  set(OS_LIBS crypt32)
endif()

# SSH (optional via vcpkg)
if (GRA_ENABLE_SSH)
  find_path(LIBSSH2_INCLUDE_DIR NAMES libssh2.h)
  find_library(LIBSSH2_LIBRARY NAMES ssh2 libssh2)
  if (LIBSSH2_INCLUDE_DIR AND LIBSSH2_LIBRARY)
    add_compile_definitions(GRA_HAS_LIBSSH2=1)
    set(SSH_LIB ${LIBSSH2_LIBRARY})
    include_directories(${LIBSSH2_INCLUDE_DIR})
  else()
    message(WARNING "libssh2 not found; SSH features disabled")
  endif()
endif()

file(GLOB SRC_UI     "src/ui/*.cpp" "src/ui/*.h")
file(GLOB SRC_CORE   "src/core/*.cpp" "src/core/*.h")
file(GLOB SRC_API    "src/api/*.cpp"  "src/api/*.h")
file(GLOB SRC_SEC    "src/security/*.cpp" "src/security/*.h")
file(GLOB SRC_UTIL   "src/util/*.h")

add_executable(gra
  src/main.cpp
  ${SRC_UI} ${SRC_CORE} ${SRC_API} ${SRC_SEC} ${SRC_UTIL}
)

target_include_directories(gra PRIVATE src)
target_link_libraries(gra PRIVATE Qt6::Widgets Qt6::Network Qt6::Sql ${OS_LIBS} ${SSH_LIB})

if (GRA_STRICT)
  if (MSVC)
    target_compile_options(gra PRIVATE /W4 /permissive-)
  else()
    target_compile_options(gra PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endif()

# === Added by FINAL pack ===
option(GRA_ENABLE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(GRA_ENABLE_SANITIZERS "Enable Address/Undefined sanitizers in Debug" ON)
if (MSVC)
  target_compile_options(gra PRIVATE /W4)
  if (GRA_ENABLE_WARNINGS_AS_ERRORS) target_compile_options(gra PRIVATE /WX) endif()
else()
  target_compile_features(gra PRIVATE cxx_std_20)
  target_compile_options(gra PRIVATE -Wall -Wextra -Wpedantic)
  if (GRA_ENABLE_WARNINGS_AS_ERRORS) target_compile_options(gra PRIVATE -Werror) endif()
  if (GRA_ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_options(gra PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(gra PRIVATE -fsanitize=address,undefined)
  endif()
endif()
