#include <gtest/gtest.h>

#include "lib/lua_templates.h"

TEST(LuaRuntime, BundledJitAndLibraries) {
  Lua lua;
  lua.to_state([](lua_State *state) {
    const int result = luaL_dostring(state, R"(
      assert(jit.version:match('^LuaJIT 2%.1'))
      assert(jit.status())
      local sum = 0
      for i = 1, 10000 do sum = sum + i end
      assert(sum == 50005000)
      assert(require('jit.util').traceinfo(1))
      assert(require('bit').band(255, 15) == 15)
      assert(require('ffi').sizeof('int32_t') == 4)
      local text = '\xe4\xbd\xa0\xe5\xa5\xbd\xf0\xa0\xae\xb7'
      assert(utf8.len(text) == 3)
      assert(utf8.codepoint(text, 7) == 0x20BB7)
      assert(utf8.offset(text, -1) == 7)
      local codes = {}
      for _, cp in utf8.codes(text) do codes[#codes + 1] = cp end
      assert(utf8.char(table.unpack(codes)) == text)
      local t = setmetatable({}, {
        __pairs = function() return next, {value = 42}, nil end
      })
      local count = 0
      for k, v in pairs(t) do
        assert(k == 'value' and v == 42)
        count = count + 1
      end
      assert(count == 1)
    )");
    ASSERT_EQ(LUA_OK, result) << lua_tostring(state, -1);
  });
}

TEST(LuaRuntime, CoroutineTranslation) {
  Lua lua;
  lua.to_state([](lua_State *state) {
    ASSERT_EQ(LUA_OK, luaL_dostring(state, R"(
      function translate()
        yield('\xe4\xbd\xa0')
        yield('\xe4\xbd\xa0\xe5\xa5\xbd')
        error('translation error')
      end
    )"));
  });
  auto thread = lua.newthread(lua.getglobal("translate"));
  for (const auto &text : {"\xe4\xbd\xa0", "\xe4\xbd\xa0\xe5\xa5\xbd"}) {
    auto result = lua.resume<std::string>(thread);
    ASSERT_TRUE(result.ok());
    EXPECT_EQ(text, result.get());
  }
  auto error = lua.resume<std::string>(thread);
  ASSERT_FALSE(error.ok());
  EXPECT_NE(std::string::npos, error.get_err().e.find("translation error"));
}

TEST(LuaRuntime, WideIntegerBinding) {
  Lua lua;
  lua.to_state([](lua_State *state) {
    constexpr unsigned long long value = 1700000000000ULL;
    LuaType<unsigned long long>::pushdata(state, value);
    EXPECT_EQ(value, LuaType<unsigned long long>::todata(state, -1));
    lua_pop(state, 1);
  });
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
