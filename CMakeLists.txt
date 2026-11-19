cmake_minimum_required(VERSION 3.24)
project(tsdb VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # lets VS Code / clangd find headers

option(TSDB_BUILD_TESTS      "Build unit tests"                ON)
option(TSDB_BUILD_BENCHMARKS "Build benchmarks"                OFF)
option(TSDB_SANITIZERS       "Enable AddressSanitizer + UBSan" OFF)

# ---- warnings / sanitizers (applied to our targets only) -------------------
add_library(tsdb_options INTERFACE)
if(MSVC)
  target_compile_options(tsdb_options INTERFACE /W4 /permissive-)
else()
  target_compile_options(tsdb_options INTERFACE -Wall -Wextra -Wpedantic -Wshadow -Wconversion)
  if(TSDB_SANITIZERS)
    target_compile_options(tsdb_options INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(tsdb_options INTERFACE -fsanitize=address,undefined)
  endif()
endif()

# ---- the library ------------------------------------------------------------
# Add new .cpp files to this list as you create them.
add_library(tsdb
  src/version.cpp
)
target_include_directories(tsdb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(tsdb PRIVATE tsdb_options)

# ---- command-line tool (src/main.cpp) ----------------------------------------
add_executable(tsdb_cli src/main.cpp)
target_link_libraries(tsdb_cli PRIVATE tsdb tsdb_options)

# ---- tests / benchmarks -------------------------------------------------------
include(FetchContent)

if(TSDB_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(TSDB_BUILD_BENCHMARKS)
  add_subdirectory(bench)
endif()
