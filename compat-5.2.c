#include "lua.h"
#include "lauxlib.h"
#include "compat-5.2.h"

/* don't compile it again if it already is include via compat-5.2.h */
#ifndef COMPAT52_C_
#define COMPAT52_C_


/* definitions for Lua 5.0 *and* Lua 5.1 */
#if !defined(LUA_VERSION_NUM) || LUA_VERSION_NUM == 501

COMPAT52_API int lua_absindex (lua_State *L, int i) {
  if (i < 0 && i > LUA_REGISTRYINDEX)
    i += lua_gettop(L) + 1;
  return i;
}


COMPAT52_API void lua_copy (lua_State *L, int from, int to) {
  int abs_to = lua_absindex(L, to);
  luaL_checkstack(L, 1, "not enough stack slots");
  lua_pushvalue(L, from);
  lua_replace(L, abs_to);
}


COMPAT52_API void lua_rawgetp (lua_State *L, int i, const void *p) {
  int abs_i = lua_absindex(L, i);
  lua_pushlightuserdata(L, (void*)p);
  lua_rawget(L, abs_i);
}

COMPAT52_API void lua_rawsetp (lua_State *L, int i, const void *p) {
  int abs_i = lua_absindex(L, i);
  luaL_checkstack(L, 1, "not enough stack slots");
  lua_pushlightuserdata(L, (void*)p);
  lua_insert(L, -2);
  lua_rawset(L, abs_i);
}


COMPAT52_API void *luaL_testudata (lua_State *L, int i, const char *tname) {
  void *p = lua_touserdata(L, i);
  luaL_checkstack(L, 2, "not enough stack slots");
  if (p == NULL || !lua_getmetatable(L, i))
    return NULL;
  else {
    int res = 0;
    luaL_getmetatable(L, tname);
    res = lua_rawequal(L, -1, -2);
    lua_pop(L, 2);
    if (!res)
      p = NULL;
  }
  return p;
}


COMPAT52_API lua_Number lua_tonumberx (lua_State *L, int i, int *isnum) {
  lua_Number n = lua_tonumber(L, i);
  if (isnum != NULL) {
    *isnum = (n != 0 || lua_isnumber(L, i));
  }
  return n;
}


COMPAT52_API void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
  luaL_checkstack(L, nup+1, "too many upvalues");
  for (; l->name != NULL; l++) {  /* fill the table with given functions */
    int i;
    lua_pushstring(L, l->name);
    for (i = 0; i < nup; i++)  /* copy upvalues to the top */
      lua_pushvalue(L, -(nup + 1));
    lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
    lua_settable(L, -(nup + 3)); /* table must be below the upvalues, the name and the closure */
  }
  lua_pop(L, nup);  /* remove upvalues */
}


COMPAT52_API void luaL_setmetatable (lua_State *L, const char *tname) {
  luaL_checkstack(L, 1, "not enough stack slots");
  luaL_getmetatable(L, tname);
  lua_setmetatable(L, -2);
}


COMPAT52_API int luaL_getsubtable (lua_State *L, int i, const char *name) {
  int abs_i = lua_absindex(L, i);
  luaL_checkstack(L, 3, "not enough stack slots");
  lua_pushstring(L, name);
  lua_gettable(L, abs_i);
  if (lua_istable(L, -1))
    return 1;
  lua_pop(L, 1);
  lua_newtable(L);
  lua_pushstring(L, name);
  lua_pushvalue(L, -2);
  lua_settable(L, abs_i);
  return 0;
}

#endif /* Lua 5.0 and Lua 5.1 */



/* definitions for Lua 5.1 only */
#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM == 501
#include <limits.h>

typedef LUAI_INT32 LUA_INT32;

/********************************************************************/
/*                    extract of 5.2's luaconf.h                    */
/*  detects proper defines for faster unsigned<->number conversion  */
/*           see copyright notice at the end of this file           */
/********************************************************************/

#if !defined(LUA_ANSI) && defined(_WIN32) && !defined(_WIN32_WCE)
#define LUA_WIN		/* enable goodies for regular Windows platforms */
#endif


#if defined(LUA_NUMBER_DOUBLE) && !defined(LUA_ANSI)	/* { */

/* Microsoft compiler on a Pentium (32 bit) ? */
#if defined(LUA_WIN) && defined(_MSC_VER) && defined(_M_IX86)	/* { */

#define LUA_MSASMTRICK
#define LUA_IEEEENDIAN		0
#define LUA_NANTRICK

/* pentium 32 bits? */
#elif defined(__i386__) || defined(__i386) || defined(__X86__) /* }{ */

#define LUA_IEEE754TRICK
#define LUA_IEEELL
#define LUA_IEEEENDIAN		0
#define LUA_NANTRICK

/* pentium 64 bits? */
#elif defined(__x86_64)						/* }{ */

#define LUA_IEEE754TRICK
#define LUA_IEEEENDIAN		0

#elif defined(__POWERPC__) || defined(__ppc__)			/* }{ */

#define LUA_IEEE754TRICK
#define LUA_IEEEENDIAN		1

#else								/* }{ */

/* assume IEEE754 and a 32-bit integer type */
#define LUA_IEEE754TRICK

#endif								/* } */

#endif							/* } */


/********************************************************************/
/*                    extract of 5.2's llimits.h                    */
/*       gives us lua_number2unsigned and lua_unsigned2number       */
/*           see copyright notice at the end of this file           */
/********************************************************************/

#if defined(MS_ASMTRICK) || defined(LUA_MSASMTRICK)	/* { */
/* trick with Microsoft assembler for X86 */

#define lua_number2unsigned(i,n)  \
  {__int64 l; __asm {__asm fld n   __asm fistp l} i = (unsigned int)l;}


#elif defined(LUA_IEEE754TRICK)		/* }{ */
/* the next trick should work on any machine using IEEE754 with
   a 32-bit int type */

union luai_Cast { double l_d; LUA_INT32 l_p[2]; };

#if !defined(LUA_IEEEENDIAN)	/* { */
#define LUAI_EXTRAIEEE	\
  static const union luai_Cast ieeeendian = {-(33.0 + 6755399441055744.0)};
#define LUA_IEEEENDIANLOC	(ieeeendian.l_p[1] == 33)
#else
#define LUA_IEEEENDIANLOC	LUA_IEEEENDIAN
#define LUAI_EXTRAIEEE		/* empty */
#endif				/* } */

#define lua_number2int32(i,n,t) \
  { LUAI_EXTRAIEEE \
    volatile union luai_Cast u; u.l_d = (n) + 6755399441055744.0; \
    (i) = (t)u.l_p[LUA_IEEEENDIANLOC]; }

#define lua_number2unsigned(i,n)	lua_number2int32(i, n, lua_Unsigned)

#endif				/* } */


/* the following definitions always work, but may be slow */

#if !defined(lua_number2unsigned)	/* { */
/* the following definition assures proper modulo behavior */
#if defined(LUA_NUMBER_DOUBLE) || defined(LUA_NUMBER_FLOAT)
#include <math.h>
#define LUA_SUPUNSIGNED	((lua_Number)(~(lua_Unsigned)0) + 1)
#define lua_number2unsigned(i,n)  \
	((i)=(lua_Unsigned)((n) - floor((n)/LUA_SUPUNSIGNED)*LUA_SUPUNSIGNED))
#else
#define lua_number2unsigned(i,n)	((i)=(lua_Unsigned)(n))
#endif
#endif				/* } */


#if !defined(lua_unsigned2number)
/* on several machines, coercion from unsigned to double is slow,
   so it may be worth to avoid */
#define lua_unsigned2number(u)  \
    (((u) <= (lua_Unsigned)INT_MAX) ? (lua_Number)(int)(u) : (lua_Number)(u))
#endif

/********************************************************************/


COMPAT52_API void lua_pushunsigned (lua_State *L, lua_Unsigned n) {
  lua_pushnumber(L, lua_unsigned2number(n));
}


COMPAT52_API lua_Unsigned luaL_checkunsigned (lua_State *L, int i) {
  lua_Unsigned result;
  lua_Number n = lua_tonumber(L, i);
  if (n == 0 && !lua_isnumber(L, i))
    luaL_checktype(L, i, LUA_TNUMBER);
  lua_number2unsigned(result, n);
  return result;
}


COMPAT52_API lua_Unsigned lua_tounsignedx (lua_State *L, int i, int *isnum) {
  lua_Unsigned result;
  lua_Number n = lua_tonumberx(L, i, isnum);
  lua_number2unsigned(result, n);
  return result;
}


COMPAT52_API lua_Unsigned luaL_optunsigned (lua_State *L, int i, lua_Unsigned def) {
  return luaL_opt(L, luaL_checkunsigned, i, def);
}


COMPAT52_API lua_Integer lua_tointegerx (lua_State *L, int i, int *isnum) {
  lua_Integer n = lua_tointeger(L, i);
  if (isnum != NULL) {
    *isnum = (n != 0 || lua_isnumber(L, i));
  }
  return n;
}


COMPAT52_API void lua_len (lua_State *L, int i) {
  switch (lua_type(L, i)) {
    case LUA_TSTRING: /* fall through */
    case LUA_TTABLE:
      lua_pushnumber(L, (int)lua_objlen(L, i));
      break;
    case LUA_TUSERDATA:
      if (luaL_callmeta(L, i, "__len"))
        break;
      /* maybe fall through */
    default:
      luaL_error(L, "attempt to get length of a %s value",
                 lua_typename(L, lua_type(L, i)));
  }
}


COMPAT52_API int luaL_len (lua_State *L, int i) {
  int res = 0, isnum = 0;
  luaL_checkstack(L, 1, "not enough stack slots");
  lua_len(L, i);
  res = (int)lua_tointegerx(L, -1, &isnum);
  lua_pop(L, 1);
  if (!isnum)
    luaL_error(L, "object length is not a number");
  return res;
}

#endif /* Lua 5.1 only */


#endif /* COMPAT52_C_ */


/*********************************************************************
* This file contains parts of Lua 5.2's source code:
*
* Copyright (C) 1994-2013 Lua.org, PUC-Rio.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*********************************************************************/

