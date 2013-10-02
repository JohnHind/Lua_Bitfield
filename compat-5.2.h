#ifndef COMPAT52_H_
#define COMPAT52_H_

#include "lua.h"
#include "lauxlib.h"


#if defined(COMPAT52_PREFIX)
/* - change the symbol names of functions to avoid linker conflicts
 * - compat-5.2.c needs to be compiled (and linked) separately
 */
#  if !defined(COMPAT52_API)
#    define COMPAT52_API extern
#  endif
#  undef COMPAT52_INCLUDE_SOURCE
#else /* COMPAT52_PREFIX */
/* - make all functions static and include the source.
 * - don't mess with the symbol names of functions
 * - compat-5.2.c doesn't need to be compiled (and linked) separately
 */
#  define COMPAT52_PREFIX lua
#  undef COMPAT52_API
#  if defined(__GNUC__) || defined(__clang__)
#    define COMPAT52_API __attribute__((__unused__)) static
#  else
#    define COMPAT52_API static
#  endif
#  define COMPAT52_INCLUDE_SOURCE
#endif /* COMPAT52_PREFIX */

#define COMPAT52_CONCAT_HELPER(a, b) a##b
#define COMPAT52_CONCAT(a, b) COMPAT52_CONCAT_HELPER(a, b)



/* declarations for Lua 5.0 only */
#if !defined(LUA_VERSION_NUM)

#define luaL_Reg luaL_reg

#define luaL_opt(L, f, n, d) \
  (lua_isnoneornil(L, n) ? (d) : f(L, n))

#define luaL_addchar(B,c) \
  ((void)((B)->p < ((B)->buffer+LUAL_BUFFERSIZE) || luaL_prepbuffer(B)), \
   (*(B)->p++ = (char)(c)))

#endif /* Lua 5.0 */



/* declarations for Lua 5.1 only */
#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM == 501

typedef LUAI_UINT32 lua_Unsigned;

#define lua_tounsigned(L, i) lua_tounsignedx(L, i, NULL)

#define lua_rawlen(L, i) lua_objlen(L, i)

#define lua_pushunsigned COMPAT52_CONCAT(COMPAT52_PREFIX, _pushunsigned)
COMPAT52_API void lua_pushunsigned (lua_State *L, lua_Unsigned n);

#define luaL_checkunsigned COMPAT52_CONCAT(COMPAT52_PREFIX, L_checkunsigned)
COMPAT52_API lua_Unsigned luaL_checkunsigned (lua_State *L, int i);

#define lua_tounsignedx COMPAT52_CONCAT(COMPAT52_PREFIX, _tounsignedx)
COMPAT52_API lua_Unsigned lua_tounsignedx (lua_State *L, int i, int *isnum);

#define luaL_optunsigned COMPAT52_CONCAT(COMPAT52_PREFIX, L_optunsigned)
COMPAT52_API lua_Unsigned luaL_optunsigned (lua_State *L, int i, lua_Unsigned def);

#define lua_tointegerx COMPAT52_CONCAT(COMPAT52_PREFIX, _tointegerx)
COMPAT52_API lua_Integer lua_tointegerx (lua_State *L, int i, int *isnum);

#define lua_len COMPAT52_CONCAT(COMPAT52_PREFIX, _len)
COMPAT52_API void lua_len (lua_State *L, int i);

#define luaL_len COMPAT52_CONCAT(COMPAT52_PREFIX, L_len)
COMPAT52_API int luaL_len (lua_State *L, int i);

#endif /* Lua 5.1 */



/* declarations for both Lua 5.0 *and* Lua 5.1 */
#if !defined(LUA_VERSION_NUM) || LUA_VERSION_NUM == 501

#define lua_pushglobaltable(L) \
  lua_pushvalue((L), LUA_GLOBALSINDEX)

#define luaL_newlib(L, l) \
  (lua_newtable((L)),luaL_setfuncs((L), (l), 0))

#define lua_getuservalue(L, i) lua_getfenv((L), (i))
#define lua_setuservalue(L, i) \
  (luaL_checktype((L), -1, LUA_TTABLE), lua_setfenv((L), (i)))

#define lua_absindex COMPAT52_CONCAT(COMPAT52_PREFIX, _absindex)
COMPAT52_API int lua_absindex (lua_State *L, int i);

#define lua_copy COMPAT52_CONCAT(COMPAT52_PREFIX, _copy)
COMPAT52_API void lua_copy (lua_State *L, int from, int to);

#define lua_rawgetp COMPAT52_CONCAT(COMPAT52_PREFIX, _rawgetp)
COMPAT52_API void lua_rawgetp (lua_State *L, int i, const void *p);

#define lua_rawsetp COMPAT52_CONCAT(COMPAT52_PREFIX, _rawsetp)
COMPAT52_API void lua_rawsetp (lua_State *L, int i, const void *p);

#define luaL_testudata COMPAT52_CONCAT(COMPAT52_PREFIX, L_testudata)
COMPAT52_API void *luaL_testudata (lua_State *L, int i, const char *tname);

#define lua_tonumberx COMPAT52_CONCAT(COMPAT52_PREFIX, _tonumberx)
COMPAT52_API lua_Number lua_tonumberx (lua_State *L, int i, int *isnum);

#define luaL_setfuncs COMPAT52_CONCAT(COMPAT52_PREFIX, L_setfuncs)
COMPAT52_API void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup);

#define luaL_setmetatable COMPAT52_CONCAT(COMPAT52_PREFIX, L_setmetatable)
COMPAT52_API void luaL_setmetatable (lua_State *L, const char *tname);

#define luaL_getsubtable COMPAT52_CONCAT(COMPAT52_PREFIX, L_getsubtable)
COMPAT52_API int luaL_getsubtable (lua_State *L, int i, const char *name);

#endif /* Lua 5.0 and 5.1 */


#if defined(COMPAT52_INCLUDE_SOURCE)
#  include "compat-5.2.c"
#endif


#endif /* COMPAT52_H_ */

