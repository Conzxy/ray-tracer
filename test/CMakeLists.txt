#enable_testing()
# 获取test和benchmark的所有源文件
file(GLOB RAY_TRACER_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*/*test*.cc")
file(GLOB RAY_TRACER_BENCH_SOURCES "${PROJECT_SOURCE_DIR}/test/*/*bench*.cc")

############################################
# User can determine whether to build all tests when build target all
# e.g. cmake --build */ray_tracer/build [--target all -j 2]
############################################
# 默认将test目标从all目标中排除
set(BUILD_ALL_TESTS OFF CACHE BOOL "Don't exclude tests from target all")
message(STATUS "BUILD_ALL_TESTS = ${BUILD_ALL_TESTS}")

message(STATUS "ray_tracer test source files: ${RAY_TRACER_TEST_SOURCES}")
message(STATUS "ray_tracer bench source files: ${RAY_TRACER_BENCH_SOURCES}")

if (NOT ${BUILD_ALL_TESTS})
  message(STATUS "[The ray_tracer test and bench files are excluded from the build of target all]")
endif (NOT ${BUILD_ALL_TESTS})

# 生成测试文件对应的可执行文件
# 
# \param SRC The variable including all test file(*.cc)
# \param ... Libraries
macro (GenTest SRC)
  foreach (ray_tracer_test_source ${${SRC}})
    # 将扩展名去掉(*.cc -> *)
    get_filename_component(ray_tracer_test_filename ${ray_tracer_test_source} NAME_WE)

    if (${BUILD_ALL_TESTS})
      add_executable(${ray_tracer_test_filename} ${ray_tracer_test_source})
    else ()
      message(STATUS "${ray_tracer_test_filename} is excluded from target all")
      add_executable(${ray_tracer_test_filename} EXCLUDE_FROM_ALL ${ray_tracer_test_source})
    endif (${BUILD_ALL_TESTS})

    target_link_libraries(${ray_tracer_test_filename} ${ARGN})
    set_target_properties(${ray_tracer_test_filename}
      PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
      COMMAND ${ray_tracer_test_filename}
    )

    add_test(NAME ${ray_tracer_test_filename} COMMAND ${ray_tracer_test_filename})

  endforeach (ray_tracer_test_source ${RAY_TRACER_TEST_SOURCES})
endmacro()

############################################
# generate test and bench program
############################################
# 单元测试依赖gtest
# 基准测试依赖benchmark
# 用户可根据需要替换
# 至于其他依赖项也可以在尾部追加
GenTest(RAY_TRACER_TEST_SOURCES gtest gtest_main)
# GenTest(RAY_TRACER_BENCH_SOURCES benchmark benchmark_main)
