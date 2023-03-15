local win32 = require "lrwin32"
win32.Window = require "Window"
require "utils"

local WM = win32.window_messages
local WS = win32.window_styles
local COLOR = win32.color_index

local TestLua = {
	lpszClassName = "TESTLUA",
	Message = {},
}

function TestLua.Register()
	wc = {
		style = 0,
		lpszClassName = TestLua.lpszClassName,
		wnd_proc = TestLua.WndProc,
		hbrBackground = COLOR.WINDOW + 1,
	}
	return win32.RegisterClass(wc)
end

function TestLua.Create()
	createwnd = {
		lpszClassName = TestLua.lpszClassName,
		name = "Test Lua Win32",
		style = WS.OVERLAPPEDWINDOW | WS.VISIBLE,
	}
	return win32.CreateWindowEx(createwnd)
end

TestLua.Message[WM.DESTROY] = function (hwnd, uMsg, wParam, lParam)
	win32.PostQuitMessage(0)
	return 0
end

TestLua.Message[WM.PAINT] = function (hwnd, uMsg, wParam, lParam)
	local ps = win32.BeginPaint(hwnd)

	--win32.FillRect(ps.hdc, ps.rcPaint, COLOR.WINDOW + 1)
	win32.TextOut(ps.hdc, 10, 10, "Hello World!")

	win32.EndPaint(hwnd, ps)
	return 0
end

function TestLua.WndProc(hwnd, uMsg, wParam, lParam)
	local f = default(TestLua.Message[uMsg], win32.DefWindowProc)
	local status, v = pcall(f, hwnd, uMsg, wParam, lParam)
	if not status then
		win32.MessageBox(hwnd, v, win32.GetWindowText(hwnd), win32.MB.OK | win32.MB.ICONERROR)
		v = 0
	end
	return v
end

function MessageLoop()
	local msg = {}
	while win32.GetMessage(msg) do
		win32.TranslateMessage(msg)
		win32.DispatchMessage(msg)
	end
	return msg.message
end

TestLua.Register()
TestLua.Create()

MessageLoop()
