<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>

<head>
<title>Bitfield Type Library for Lua 5</title>
<link rel="stylesheet" type="text/css" href="bitfield.css">
<META HTTP-EQUIV="content-type" CONTENT="text/html; charset=utf-8">
</head>

<body>

<hr>
<h1>
<a href="http://www.lua.org/"><img src="bitfield.gif" alt="" border="0"></a>
Bitfield Type Library for Lua 5
</h1>
by John Hind
<p>
<small>
Copyright &copy; 2013.
Freely available under the terms of the
<a href="http://www.lua.org/license.html">Lua license</a>.
</small>
<hr>
<p>

##Introduction

A bitfield is an add-in Lua type provided by the `bitfield.dll` / `bitfield.so` run-time loadable library. It is implemented as a subtype of userdata which stores a vector of bits (or booleans) between 1 and 256 bits wide. The state of the bits in a bitfield is mutable, but the width is not (it is established when the bitfield is created).

This library is offered as an alternative to the standard Lua `bit32` library. The primary applications targeted are the implementation of communications protocols and the representation of bit-mapped registers in hardware devices.

##Getting Started

Common header code required for all examples:

	local bf = require("bitfield")
	bitfield = bf.bitfield
	bitrange = bf.bitrange

Creating a bitfield:

	bf = bitfield(4)

This creates a bitfield four bits wide initialised all `0` (`false`). The bits are indexed from `1` up to the bit width, which is the value of the `#` operator (4 in this case). Indexing a bitfield returns the state of the indexed bit as a boolean, or sets the indexed bit from a boolean:

	bf[4] = true -- set bit 4 to true (1)
	bf[1] = not bf[1] -- toggle bit 1
	for i=1, #bf do bf[i] = not bf[i] end

There are no multi-bit logical operations, instead operations of arbitrary complexity can be constructed using Lua boolean operations and `for` loops in combination with bitfield indexing as in the last line above. Indexing a bit beyond the width of the bitfield reads `false` and writes nothing.

Equality is defined so two bitfields are equal if they are the same width and the corresponding bits are all in the same state.

A `__tostring` metamethod is defined to convert the bitfield into a binary string format, enabling the standard Lua `tostring` and `print` functions. A `__type` metafield has the value "bitfield" which can be used to determine if a userdata variable is a bitfield.

	bf1 = bitfield(4)
	bf2 = bitfield(4)
	print(tostring(bf1 == bf2)) -- true
	bf2[2] = true
	print(tostring(bf1 == bf2)) -- false
	print(tostring(bf1 ~= bf2)) -- true
	print(bf1) -- 0010
	print(tostring(bf1) == "0010") -- true

	typeof = function(x,n)
		local t, m = type(x), {}
		if t == n then return true end
		if t == "userdata" or t == "table" then
			m = getmetatable(x)
			if m then m = m.__type end
			if m then return m == n end
		end
		return false
	end
	print(tostring(typeof(bf1,"bitfield")) -- true

##Initialisation

Bitfields may be initialised or copied on creation:

	bf = bitfield(4,"1") -- 4-bit field "1111" (binary string is repeated)
	bf = bitfield(12,123) -- 12-bit field "000001111011"
	bf = bitfield("0110") -- 4-bit field "0110"
	b1 = bitfield(bf) -- bitfield b1 created as a copy of bitfield bf
	bf = bitfield(true,false,true) -- 3-bit field "101"

When initialising from a number, the width must be specified either explicitly or by specifying another bitfield as a template. Alternatively a list of booleans, bitfields and/or binary strings may be used since the width of these types is explicit.

It is also possible to create and initialise a bitfield by concatenation of boolean and/or bitfield values:

	bf = true .. bitfield("1010") .. false .. true .. bitfield(4,"1")
	-- result: 11010011111

However the list form of the bitfield function may be preferred as it is usually more efficient.

##Ranges

Individual bits can be indexed as boolean values using a number index as already seen. Also available is a range index which can specify an individual bit or a contiguous range of bits and may also specify a type to represent the indexed bits.

	bf = bitfield("10011001")
	print(tostring(bf[bitrange(3)])) -- false (bit 3 as boolean)
	print(bf[bitrange(3,6)]) -- 6 (bits 3 through 6 as number)

The indexed value is boolean by default for a single bit, or number (unsigned) for 2 upto the width of `lua_Unsigned` (32-bits in the standard Lua build), or binary (string). This may be changed to "boolean" (only for single bit ranges), "binary" (string), "bitfield", "unsigned" (number) or "packed" (string - see next section).

	print(bf[bitrange("bitfield",1,4)]:tostring()) -- bitfield "1001"
	print(bf[bitrange("binary",1,4)]) -- "1001" (a binary string)
	print(bf[bitrange("unsigned",1)]) -- 1 (an unsigned number)

Ranges can also be used to change bits in bitfields:

	bf[bitrange(1,4)] = 9
	bf[bitrange("binary",1,4)] = "1001"

A full-width range may be used to convert the entire bitfield to and from other formats:

	n = bf[bitrange("unsigned")] -- 153
	bf[bitrange("binary")] == tostring(bf) -- true

##Packed Format

The "packed" format is a non-printable string in which eight bits are packed into each character. The right most character represents up to eight least significant bits with the least significant having value 1. The left-most character has any unused bits set to zero, so it is not always possible to tell the exact width of the bitfield from the packed string alone.

	bf = bitfield("101010101010101010101")
	pl = #bf
	ps = bf[bitrange("packed")]
	-- later
	bf = bitfield(pl)
	bf[bitrange("packed")] = ps

##Named Ranges

Ranges may be named and used to reference parts of bitfields using dot syntax analogous to the way fields in tables are referenced in Lua.

	bf = bitfield{
		flag1 = bitrange(1),
		flag2 = bitrange(2),
		count = bitrange(3,6),
		payload = bitrange("bitfield",7,32) }
	bf.flag1 = true
	bf.flag2 = not bf.flag1
	bf.count = 8
	bf.payload = bitfield(26,5436)

The width of the bitfield is determined from the maximum index found in the Named Range Table (32 bits wide in this example). If the ranges do not cover all the bits in the desired width, simply include a dummy range that spans the entire width. Ranges may overlap allowing constructs similar to unions in C. Note that the names of methods of bitfield (`tostring`, `tonumber`) may not be used as range names.

A common Named Range Table may be referenced by multiple bitfields:

	rt = {  _ = bitrange(1,32), flag1 = bitrange(1) }
	b1 = bitfield(rt) -- 32 bits initialised all 0
	b2 = bitfield(rt, "1") -- 32 bits initialised all 1
	b3 = bitfield(rt, 23) -- same range table, lowest bits '10111'
	b4 = bitfield(b1) -- copy of b1 referencing the same range table

This feature provides a simple type system for bitfields.

##Named Constants

The Named Range Table may also include named constants. Constant names are resolved when a value is assigned to a range as follows:

	nrt = {
		dir = bitrange(1)
		flag = bitrange(2)
		on = bitrange("uconst", true)
		off = bitrange("uconst", false)
		output = bitrange("const", 1, "0")
	}
	bf = bitfield(nrt)
	bf.dir = 'output'
	bf.flag = 'on'

There are two types of constant: "const" are bound to a specific range and "uconst" may be assigned to any range provided it is the same width. In the example above 'output' may be used only to set bit 1 of the bitfield to `0` while 'on' may be used to set any single bit to `1`.

Constants may also be used in the creation of a bitfield. Using the Named Range Table from the example above:

	bf = bitfield(nrt, 'output', 'on', 'off', 'on') -- "0101"

The list of constants is read from right to left and an internal index keeps track of the current position, starting at index `1`. A bound constant sets its bound range and advances the cursor to the next index after that range. An unbound constant sets a range beginning at the cursor and advances the cursor by the width of the constant. The list may be mixed with other initialisers.

##Reference

### bitfield
`bitfield ( [template,][ init ...] )`

Creates, initialises and returns a new bitfield.

`template` may be the width (a number between 1 and 256), a bitfield to be copied, a Named Range Table (NRT) or it may be omitted provided the initialiser list contains no numbers and does not start with a bitfield. If a bitfield is copied, it will have the same width and NRT (if any) as the original. It will also have the same values initially, but these may be overwritten by initialisers. In all other cases, the bitfield is initialised all `false` prior to application of initialisers.

If used, the NRT must be a Lua table in which the keys are strings obaying the restrictions for Lua identifiers and the values are range keys returned by `bitrange` (see below). The width of the produced bitfield is determined from the largest index used in the range keys.

`init` is a list of zero or more initialisation values which are applied right to left. An internal index cursor is initialised to `1`. If an initialiser specifies bits outside the width, those bits are discarded.

A binary string value is inserted at the cursor which is then incremented by the length of the string. If the binary string is at the left end of the list, it is duplicated to fill the rest of the width to the left.

A bitfield is copied into the new bitfield at the cursor which is then incremented by the width of the copied bitfield.

A boolean value sets or resets the bit at the cursor which is incremented by one.

A number (unsigned) may only be used in the left most list position and fills the remaining width from the cursor.

A string which is the name of a constant in the NRT may also be used. An unbound constant is inserted at the cursor which is then incremented by the width. A bound constant is inserted in its bound range and the cursor moved to the end of the bound range plus one.

* * * * *

### bitrange
`bitrange ( [type,][ first[, last]][, val] )`

Returns a range key (implemented as an opaque four character string) or a constant which is a range key followed by a value in packed format (concatenated into a single string). Use this function as the index to a bitfield or to create the value for an entry in a Named Range Table.

If `type` is supplied it must be one of the string keys "boolean", "bitfield", "binary", "unsigned", "packed", "const" or "uconst". If omitted, defaults to "boolean" for a single bit range, to "unsigned" for a multi-bit range of that fits in the `lua_Unsigned` type (32 bits or less in the standard Lua build), otherwise "binary".

If `first` is omitted, the range covers the entire width of the bitfield. If supplied, `first` must be a number `1..256` specifying the starting index of the range. `last` must be `first..256` or omitted, in which case it defaults to `first` producing a single bit range.

`val` is ignored unless `type` is "const" or "uconst". It may be a boolean, a binary string or a number (unsigned). For "const", `first` must also be provided and `last` must be provided if `val` is a number. For "uconst" `first` is interpreted as the length in bits and need only be provided if `val` is a number. `last` must be omitted in this case.

##Extensions

The bitfield metatable is estensible to add methods and metamethods in Lua. To get the metatable:

	mt = getmetatable(bitfield(1))

It is not necessary (or desirable) to provide an `__index` metamethod in order to add methods. The standard metamethod checks string keys against the metatable and returns the value if found. Only string keys not present in the metatable are processed as range names. This means that the names of any methods added to the metatable cannot be used as range names.

One interesting possibility is to define `__add` etc. metamethods to re-purpose the arithmetic operators to express bit-wise logical operations. It would (arguably) be reasonably natural for unary `-` to mean `not`, binary `-` to mean `nor`, `+` to mean `or`, and `*` to mean `and`. 

<HR>
<SMALL CLASS="footer">
Last update:
Fri Sep 6 13:50:00 GMT 2013
</SMALL>
<!--
-->
