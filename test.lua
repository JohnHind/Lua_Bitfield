local bf = require("bitfield")
bitfield = bf.bitfield
bitrange = bf.bitrange

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

print("** Test: " .. bf._VERSION .. " **")

print("** Initialise **")

v = "11001" t = bitfield(v)
print(v, tostring(t), #t, tostring(typeof(t, "bitfield")))

t = bitfield(true, false, false)
print("100", tostring(t), #t)

v = 12 t = bitfield(6, v)
print(v, t[bitrange("unsigned")], #t)

print("101011", tostring(bitfield(6,bitfield("101"),bitfield("011"))))

print("00", bitfield(2))

print("11", bitfield(2,"1"))

print("** Concatenate **")

t = true .. false .. true .. false
print("1010", tostring(t), #t)

t = bitfield("10") .. true .. bitfield("0")
print("1010", tostring(t), #t)

print("** Full length, mutate, compare **")

t1 = bitfield(256,"10")

t2 = bitfield(t1)
print(tostring(t1))
print("t1 == t2", tostring(t1 == t2))
for i=1, 256 do t2[i] = not t1[i] end
print(tostring(t2))
print("t1 == t2", tostring(t1 == t2))

print("** Mutate **")

t1 = bitfield("1101110")
t1[3] = bitfield(1)
print("1101010", tostring(t1))

print("** Range indexing **")

t1 = bitfield("10011111001")
print(9, t1[bitrange("unsigned",1,4)])
print("true", tostring(t1[bitrange(7)]))
t1[bitrange(7)] = false
print("false", tostring(t1[bitrange(7)]))
t1[bitrange("bitfield", 8, 11)] = bitfield("0110")
print("0110", tostring(t1[bitrange("bitfield", 8, 11)]))
t = bitfield(6)
t[bitrange("binary",1,4)] = "1010"
print("001010", t)

f1 = bitfield("1010101010")
v = f1[bitrange("packed")]
f2 = bitfield(#f1)
f2[bitrange("packed")] = v
print(f1, f2, #v)

print("** Range names **")

f1 = bitfield{
	flag1 = bitrange(1),
	flag2 = bitrange(2),
	count = bitrange(3,14),
	subr  = bitrange("bitfield",15,16)
}

print(#f1)

f1.flag1 = true
f1.count = 2345
f1.subr = bitfield("01")

print("true", tostring(f1.flag1))
print(2345, tostring(f1.count))
print("01", tostring(f1.subr))

print('** Named Constants **')

nt = {
	flag1 = bitrange(1),
	count = bitrange(2,5),
	on = bitrange("uconst",true),
	off = bitrange("uconst",false),
	val = bitrange("const",2,5,"1110")
}

f1 = bitfield(nt)

print("false", tostring(f1.flag1))
f1.flag1 = 'on'
print("true", tostring(f1.flag1))
f1.count = 'val'
print("14", tostring(f1.count))

print("11100", bitfield(nt,'val'))
print("11101", bitfield(nt,'val', "1"))
