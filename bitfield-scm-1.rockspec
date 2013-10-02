package = "Bitfield"
version = "scm-1"

local tag = "master"
source = {
   url = "git://github.com/JohnHind/Lua_Bitfield/"..tag.."/bitfield.git",
   -- tag = tag,
}

description = {
   summary = "Packed Bitfield Type Library for Lua",
   detailed = [[
      Alternative to the bit32 library, 'bitfield' is an indexible vector
      of up to 256 booleans which may be converted to unsigned number,
      packed string or binary string representation. It aims to be a more
      comfortable representation of bit-mapped registers or bit-packed
      communications protocols.
   ]],
   license = "MIT/X11",
   homepage = "https://github.com/JohnHind/Lua_Bitfield",
}

dependencies = {
   "lua >= 5.1",
}

build = {
   type = "builtin",
   modules = {
      bitfield = {
         sources = {"bitfield.c","compat-5.2.c"},
      }
   }
}
