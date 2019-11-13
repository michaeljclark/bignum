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

add_library(bignum src/bignum.cpp)

enable_testing()

add_executable(test_1 tests/test_1.cpp)
target_link_libraries(test_1 bignum)
add_test(TEST_1 test_1)

add_executable(test_2 tests/test_2.cpp)
target_link_libraries(test_2 bignum)
add_test(TEST_2 test_2)

if(GMP_LIBRARY)
add_executable(bench_1 tests/bench_1.cpp)
target_link_libraries(bench_1 bignum ${GMP_LIBRARY})
endif()