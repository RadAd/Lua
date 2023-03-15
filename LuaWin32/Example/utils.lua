
function default(value, default_value)
	if value == nil then return default_value end
	return value
end

function quote(x)
	if not x then return x end
	return "\"" .. x .. "\""
end

function hexstr(x)
	if x == nil then return nil end
	return string.format("%#010x", x)
end

function test(a, b)
	return (a & b) == b;
end

function fif(condition, if_true, if_false)
  if condition then return if_true else return if_false end
end

function ternary(condition, if_true, if_false)
  return condition and if_true or if_false
end

function printtable(t, x)
	x = default(x, "")
	for i,v in pairs(t) do
		print(x.."   -", i, v)
		if type(v) == "table" then
			printtable(v, "  "..x)
		end
	end
end
