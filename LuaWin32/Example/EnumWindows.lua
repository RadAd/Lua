local win32 = require "lrwin32"
require "utils"

--print "--- EnumWindows ---"

x = win32.EnumWindows()
for i,wnd in ipairs(x) do
	style = win32.GetWindowLongPtr(wnd, win32.GWLP.STYLE)
	if test(style, win32.window_styles.CAPTION | win32.window_styles.VISIBLE) then
		print(hexstr(wnd), hexstr(style), quote(win32.GetWindowText(wnd)), quote(win32.GetClassName(wnd)))
	end
end
