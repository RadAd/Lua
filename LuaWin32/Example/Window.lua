local win32 = require "lrwin32"

local Window = {}

function Window:__tostring(o)
	return "Handle: " .. hexstr(self.handle);
end

function Window:__index(key)
	local f = Window["get_"..key]
	assert(f, "'"..key.."'' is not a valid property")
	return f(self, value)
end

function Window:__newindex(key, value)
	local f = Window["set_"..key]
	assert(f, "'"..key.."'' is a read only value")
	return f(self, value)
end

function Window.new(h)
	assert(self == nil, "self is not nil")
	return setmetatable( {
		handle = h
	}, Window)
end

function Window.foreground()
	assert(self == nil, "self is not nil")
	return Window.new(win32.GetForegroundWindow())
end

function Window.get_title(wnd)
	return win32.GetWindowText(wnd.handle);
end

function Window.set_title(wnd, t)
	win32.SetWindowText(wnd.handle, t);
end

function Window.get_class(wnd)
	return win32.GetClassName(wnd.handle);
end

function Window.get_style(wnd)
	return win32.GetWindowLongPtr(wnd.handle, win32.GWLP.STYLE);
end

return Window
