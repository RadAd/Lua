local win32 = require "lrwin32"
win32.Window = require "Window"
require "utils"

local wnd = win32.Window.foreground()

print("wnd", wnd)
print("title", wnd.title)
print("class", wnd.class)
print("style", hexstr(wnd.style))
local s = wnd.style
for i,v in pairs(win32.window_styles) do
	if test(s, v) then
		print("", i)
	end
end

--wnd.title = "test"
