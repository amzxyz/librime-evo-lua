#ifndef LUA_COMPAT_H
#define LUA_COMPAT_H

#define xlua_resume lua_resume
#define lua_rawlen lua_objlen

void xluaopen_utf8(lua_State*);

#endif /* LUA_COMPAT_H */
