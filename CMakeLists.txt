cmake_minimum_required(VERSION 3.10)
# 以便clangd可以进行语法语义分析
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# 设置项目名
# 至于版本和描述，自己根据需要设置
# project(<PROJECT-NAME>
#         [VERSION <major>[.<minor>[.<patch>[.<tweak>]]]]
#         [DESCRIPTION <project-description-string>]
#         [HOMEPAGE_URL <url-string>]
#         [LANGUAGES <language-name>...])
project(ray_tracer)

# 如果没有指定构筑类型
# 默认是Release模式
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS  "/DWIN32 /D_WINDOWS /W0 /GR /EHsc")
  set(CMAKE_CXX_FLAGS_DEBUG   "/MDd /Zi /Ob0 /Od /RTC1")
  set(CMAKE_CXX_FLAGS_RELEASE  "/MD /O2 /Ob2 /DNDEBUG")
  set(CMAKE_CXX_FLAGS_MINSIZEREL  "/MD /O1 /Ob1 /DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MD /Zi /O2 /Ob1 /DNDEBUG")
else ()
  # FYI https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Og")

  # FYI https://gcc.gnu.org/onlinedocs/gcc-5.5.0/gcc/Link-Options.html#Link-Options
  # FYI https://gcc.gnu.org/onlinedocs/gcc-5.5.0/gcc/Warning-Options.html#Warning-Options

  set(CXX_FLAGS 
  -Wall
  -Wextra
  -Wno-return-local-addr
  -Wno-unused-parameter
  -Wno-unused-function
  -Wno-switch
  -Wno-format-security
  # support INT2DOUBLE
  -Wno-strict-aliasing
  -Werror
  # make non-trivial(but like "trivial") class can reallocate
  -Wno-class-memaccess
  -Wno-implicit-fallthrough
  -Wconversion
  -Wshadow
  -Wno-maybe-uninitialized
  -Wwrite-strings # in fact, this is default specified
  -pthread
      
  # linker opt
  -rdynamic
  # machine opt
  -march=native
  )

  # Clang和GCC有些选项是不通用的
  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    list(REMOVE_ITEM CXX_FLAGS "-Wno-return-local-addr")
    list(REMOVE_ITEM CXX_FLAGS "-rdynamic")
    list(APPEND CXX_FLAGS "-Wthread-safety")
  endif()

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
endif ()

set(CMAKE_CXX_STANDARD 20)

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "CMAKE_CXX_FLAGS_DEBUG")
elseif (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
  message(STATUS "CMAKE_CXX_FLAGS_MINSIZEREL: ${CMAKE_CXX_FLAGS_MINSIZEREL}")
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  message(STATUS "CMAKE_CXX_FLAGS_RELWITHDEBINFO: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
endif ()

set(RAY_TRACER_SOURCE_DIR ${PROJECT_SOURCE_DIR}/ray_tracer)

# 如果项目依赖于第三方库，可以将以下有关third-party的均解注释
set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/third-party)
include_directories(${THIRD_PARTY_DIR})
# 可选项，不过默认包含
# 引用自己的模块，我偏向于
# #include "$project_name/xxx/xxx.h"
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/src)

# I don't want to use the builtin variable ${BUILD_SHARED_LIBS}
# since it you don't set it to ON explicitly, then the NOT 
# ${BUILD_SHARED_LIBS} will be false, and generated libraries 
# are static. That's not I wanted behavior. I want a variable,
# it can build shared libraries default even though I don't set 
# it explitly, and the build of static libraries is an option.

# 默认是生成动态库
# \waring 不要用BUILD_SHARED_LIBS
set(BUILD_STATIC_LIBS OFF CACHE BOOL "Build static libraries")

# 设置库的名称，影响在CMake文件的变量名和生成的库文件名
# 一般用于库（轮子）项目
# 
# \param var Variable name
# \param prefix The prefix of the library name
# \example
#  SetLibName(XXX_LIB xxx)
#  将生成libxxx.so(release)/libxxx_debug.so(debug)
#  在CMakeLists.txt中可以用${XXX_LIB}统一引用它们
macro (SetLibName var prefix)
  if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(${var} ${prefix})
  else ()
    set(${var} "${prefix}_debug")
  endif ()
endmacro ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set (SHARED_LIB_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin")
else ()
  set (SHARED_LIB_OUTPUT_DIR "${CMAKE_BINARY_DIR}/lib")
endif ()

set(STATIC_LIB_OUTPUT_DIR "${CMAKE_BINARY_DIR}/lib")

message(STATUS "static libraries output path: ${STATIC_LIB_OUTPUT_DIR}")
message(STATUS "shared libraries output path: ${SHARED_LIB_OUTPUT_DIR}")

# 生成库文件
# \param lib The library variable
# \param ... The source files
macro (GenLib lib)
  #if (NOT ${BUILD_SHARED_LIBS})
  message(STATUS "Source list: ${ARGN}")
  if (${BUILD_STATIC_LIBS})
    message(STATUS "Build static library: ${lib}")
    add_library(${lib} STATIC ${ARGN})
    set_target_properties(${lib}
      PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY ${STATIC_LIB_OUTPUT_DIR}
    )
  else ()
    message(STATUS "Build shared library: ${lib}")
    add_library(${lib} SHARED ${ARGN})
    # add_library(${lib} SHARED IMPORTED )
    # set_property(TARGET ${lib} PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/${lib}.dll")
    # set_property(TARGET ${lib} PROPERTY IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/${lib}.lib")

    set_target_properties(${lib}
      PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY ${SHARED_LIB_OUTPUT_DIR}
    )
  endif (${BUILD_STATIC_LIBS})
endmacro ()

# 生成可执行程序
# \param app_name The name of application program
# \param src_list The variable name including source files
# \param lib_list The variable name including library files
macro (GenApplication app_name src_list lib_list)
  set(SRC_LIST ${${src_list}})
  set(LIB_LIST ${${lib_list}})
  message(STATUS "application: ${app_name}")
  message(STATUS "Source list: ${SRC_LIST}")

  add_executable(${app_name} EXCLUDE_FROM_ALL ${SRC_LIST})
  list (LENGTH LIB_LIST lib_list_len)
  if (NOT ${lib_list_len} EQUAL 0)
    message(STATUS "Lib list: ${LIB_LIST}")
    target_link_libraries(${app_name} PRIVATE ${LIB_LIST})
  endif ()

  set_target_properties(${app_name}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
  )
endmacro()

macro (GenExec exec src_list lib_list)
  GenApplication(${exec} ${src_list} ${lib_list})
endmacro ()

# set(EXAMPLE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/example)

add_subdirectory(src)
add_subdirectory(test)
# add_subdirectory(third-party)
#add_subdirectory(example)