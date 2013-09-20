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

local mt = getmetatable(bitfield(1))
mt.__unm = function(bf)
	local b = bitfield(bf)
	for i=1, #b do b[i] = not b[i] end
	return b
end
mt.__add = function(b1, b2)
	local w = #b1
	if #b2 < w then w = #b2 end
	local b = bitfield(w)
	for i=1, w do b[i] = b1[i] or b2[i] end
	return b
end
mt.__sub = function(b1, b2)
	local w = #b1
	if #b2 < w then w = #b2 end
	local b = bitfield(w)
	for i=1, w do b[i] = not (b1[i] or b2[i]) end
	return b
end
mt.__mul = function(b1, b2)
	local w = #b1
	if #b2 < w then w = #b2 end
	local b = bitfield(w)
	for i=1, w do b[i] = b1[i] and b2[i] end
	return b
end
mt.tostring = function(bf)
	return bf[bitrange("binary")]
end

ba = bitfield("1001")
bb = bitfield("0110")
print(ba + bb)
print(ba - bb)
print(-ba)
print(ba * bb)
print(ba:tostring())
