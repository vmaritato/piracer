cmake_minimum_required(VERSION 3.18)
project(piracer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(PIRACER_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(PIRACER_ENABLE_LTO "Enable Link-Time Optimization" OFF)

# Dependencies via pkg-config (portable across Linux/macOS; for Windows add vcpkg later)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GMP REQUIRED IMPORTED_TARGET gmp)
pkg_check_modules(MPFR REQUIRED IMPORTED_TARGET mpfr)

# Warnings
if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
  if (PIRACER_WARNINGS_AS_ERRORS)
    add_compile_options(-Werror)
  endif()
endif()

# LTO (optional)
if(PIRACER_ENABLE_LTO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT lto_supported)
  if(lto_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
  endif()
endif()

# Targets
add_executable(piracer
  src/cli/main.cpp
)

target_include_directories(piracer PRIVATE ${GMP_INCLUDE_DIRS} ${MPFR_INCLUDE_DIRS} include)

target_link_libraries(piracer PRIVATE PkgConfig::GMP PkgConfig::MPFR)

# Install
install(TARGETS piracer RUNTIME DESTINATION bin)