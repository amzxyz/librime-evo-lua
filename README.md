# librime-evo-lua

Lua processors, segmentors, translators and filters for
[librime-evo](https://github.com/amzxyz/librime-evo), derived from
[librime-lua](https://github.com/hchunhui/librime-lua).

This repository follows librime-evo's dictionary, translator and user dictionary
interfaces. It is maintained and tested against librime-evo.

LuaJIT is the only runtime. Its source is a pinned submodule at
`thirdparty/luajit` and is compiled into the Lua plugin, including static librime
builds. No system Lua installation or separate Lua runtime library is required.
Building requires CMake 3.20 or newer, GNU Make on POSIX/MinGW, and a Visual Studio
developer environment for Windows MSVC/clang. macOS universal builds compile
and combine the target architectures with `lipo`.

Clone librime-evo with its submodules:

```sh
git clone --recurse-submodules https://github.com/amzxyz/librime-evo.git
```

When working directly in this repository, initialize the runtime source:

```sh
git submodule update --init --recursive
```

To upgrade LuaJIT, select a tested commit or update the `v2.1` branch, then
commit the new submodule revision:

```sh
git submodule update --remote --checkout thirdparty/luajit
# Build and run the runtime and input regressions before committing.
git add thirdparty/luajit
git commit -m "build: update LuaJIT"
```

Builds always use the recorded revision and do not download or select a Lua
implementation during CMake configuration. LuaJIT upgrades need script
regressions: newer LuaJIT bitwise syntax uses 32-bit semantics and can fool
feature probes intended to identify Lua 5.3's 64-bit operators.

The runtime exposes Lua 5.1 with LuaJIT's Lua 5.2 compatibility extensions.
The bindings also provide `utf8` and `table.unpack`. Standard Lua bytecode is
not interchangeable with LuaJIT bytecode; distribute scripts as source.

The CI builds this repository with librime-evo using GCC and Clang and runs the
core and Lua runtime tests. Evo's CI covers Windows and macOS integration.

### System random bytes and Unix time

- `rime_api.secure_random_bytes(length)` returns a binary Lua string of
  0–65536 bytes from the operating system's cryptographic RNG, or nil on
  system failure. Invalid lengths raise a Lua argument error. No PRNG fallback.
  Windows uses BCryptGenRandom, Linux/Android use getrandom, and macOS uses
  arc4random_buf.
- `rime_api.get_unix_time_ms()` returns Unix epoch milliseconds from the wall
  clock. The existing `get_time_ms()` remains a monotonic timer.
