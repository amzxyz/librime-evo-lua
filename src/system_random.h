#ifndef RIME_LUA_SYSTEM_RANDOM_H_
#define RIME_LUA_SYSTEM_RANDOM_H_

struct lua_State;

namespace rime_lua {

int SecureRandomBytes(lua_State* L);
int GetUnixTimeMs(lua_State* L);

}  // namespace rime_lua

#endif  // RIME_LUA_SYSTEM_RANDOM_H_
