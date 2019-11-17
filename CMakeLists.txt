cmake_minimum_required(VERSION 3.0.2)

project(bignum)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckCXXCompilerFlag)

find_path(GMP_INCLUDE_DIR gmp.h
    PATHS ${GMP_PREFIX}/include /usr/include /usr/local/include )
find_library(GMP_LIBRARY gmp)
if(GMP_INCLUDE_DIR)
include_directories(${GMP_INCLUDE_DIR})
endif()

# We need haswell for TZCNT/LZCNT
check_cxx_compiler_flag("-march=haswell" has_march_haswell "int main() { return 0; }")
if (has_march_haswell)
	list(APPEND CMAKE_CXX_FLAGS -march=haswell)
endif()

include_directories(src)

add_library(bignum src/bignum.cc)

enable_testing()

add_executable(test_bignum tests/test_bignum.cc)
target_link_libraries(test_bignum bignum)
add_test(TEST_BIGNUM test_bignum)

add_executable(test_wideint tests/test_wideint.cc)
target_link_libraries(test_wideint bignum)
add_test(TEST_WIDEINT test_wideint)

if(GMP_LIBRARY)
add_executable(bench_bignum tests/bench_bignum.cc)
target_link_libraries(bench_bignum bignum ${GMP_LIBRARY})
endif()
