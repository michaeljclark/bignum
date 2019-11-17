cmake_minimum_required(VERSION 3.0.2)

project(bignum)

include(CheckCXXCompilerFlag)

find_library(GMP_LIBRARY gmp)

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

if(GMP_LIBRARY)
add_executable(bench_bignum tests/bench_bignum.cc)
target_link_libraries(bench_bignum bignum ${GMP_LIBRARY})
endif()
