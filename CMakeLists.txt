cmake_minimum_required(VERSION 3.20)

include(cmake/LuaJIT.cmake)

set(random_system_libraries)
if(WIN32)
  set(random_system_libraries bcrypt)
endif()

aux_source_directory(src RIME_LUA_SRC)
aux_source_directory(src/lib RIME_LUA_LIB_SRC)

add_library(rime-lua-objs OBJECT ${RIME_LUA_SRC} ${RIME_LUA_LIB_SRC})
target_include_directories(rime-lua-objs PRIVATE thirdparty/luajit/src)
add_dependencies(rime-lua-objs rime-luajit-build)
if(BUILD_SHARED_LIBS)
  set_target_properties(rime-lua-objs PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

set(plugin_name "rime-lua" PARENT_SCOPE)
set(plugin_objs $<TARGET_OBJECTS:rime-lua-objs> $<TARGET_OBJECTS:rime-luajit-objs> PARENT_SCOPE)
set(plugin_deps ${rime_library} ${rime_gears_library} ${luajit_system_libraries} ${random_system_libraries} PARENT_SCOPE)
set(plugin_modules "lua" PARENT_SCOPE)

if(BUILD_TEST)
  add_executable(lua_runtime_test test/runtime_test.cc src/system_random.cc src/lib/lua.cc
                 src/lib/lutf8lib-compat.c $<TARGET_OBJECTS:rime-luajit-objs>)
  target_include_directories(lua_runtime_test PRIVATE src thirdparty/luajit/src)
  add_dependencies(lua_runtime_test rime-luajit-build)
  target_link_libraries(lua_runtime_test PRIVATE ${luajit_system_libraries} ${random_system_libraries} ${GTEST_LIBRARIES})
  add_test(NAME lua_runtime_test COMMAND lua_runtime_test)
endif()
