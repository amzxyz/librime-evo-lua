#include "system_random.h"

#include <chrono>
#include <cstddef>
#include <string>

#if defined(_WIN32)
#include <windows.h>
#include <bcrypt.h>
#elif defined(__linux__)
#include <cerrno>
#include <sys/syscall.h>
#include <unistd.h>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || \
    defined(__NetBSD__)
#include <stdlib.h>
#else
#include <cstdio>
#endif

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

namespace rime_lua {
namespace {

bool FillRandom(char* data, std::size_t size) {
#if defined(_WIN32)
  return BCryptGenRandom(nullptr, reinterpret_cast<PUCHAR>(data),
                         static_cast<ULONG>(size),
                         BCRYPT_USE_SYSTEM_PREFERRED_RNG) == 0;
#elif defined(__linux__)
  // The syscall also works with Android NDKs that lack a getrandom wrapper.
  while (size > 0) {
    const auto count = syscall(SYS_getrandom, data, size, 0);
    if (count < 0 && errno == EINTR)
      continue;
    if (count <= 0)
      return false;
    data += count;
    size -= static_cast<std::size_t>(count);
  }
  return true;
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || \
    defined(__NetBSD__)
  arc4random_buf(data, size);
  return true;
#else
  auto* file = std::fopen("/dev/urandom", "rb");
  if (!file)
    return false;
  const bool ok = std::fread(data, 1, size, file) == size;
  std::fclose(file);
  return ok;
#endif
}

}  // namespace

int SecureRandomBytes(lua_State* L) {
  constexpr int kMaxBytes = 65536;
  const auto length = luaL_checknumber(L, 1);
  luaL_argcheck(L, length >= 0 && length <= kMaxBytes, 1,
                "length must be between 0 and 65536");
  const auto size = static_cast<std::size_t>(length);
  luaL_argcheck(L, length == size, 1, "length must be an integer");
  std::string bytes(size, '\0');
  if (size > 0 && !FillRandom(&bytes[0], size)) {
    lua_pushnil(L);
    return 1;
  }
  lua_pushlstring(L, bytes.data(), bytes.size());
  return 1;
}

int GetUnixTimeMs(lua_State* L) {
  const auto now = std::chrono::system_clock::now().time_since_epoch();
  const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now);
  lua_pushnumber(L, static_cast<lua_Number>(millis.count()));
  return 1;
}

}  // namespace rime_lua
