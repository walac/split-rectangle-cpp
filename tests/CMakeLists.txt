add_executable(test test.cc test_rect.cc test_split.cc)
target_link_libraries(test PRIVATE Catch2::Catch2)

add_executable(fuzzy_test fuzzy_test.cc)
target_link_libraries(fuzzy_test)

add_executable(benchmark benchmark.cc)
target_link_libraries(benchmark PRIVATE Catch2::Catch2)
