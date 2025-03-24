local args = {...}
w = require "lrwin32"

skip = false
for i,arg in ipairs(args) do
    if skip then
        skip = false
    elseif arg:lower() == "/default" then
        default = args[i + 1]
        skip = true
    elseif arg:sub(1, 9):lower() == "/default=" or arg:sub(1, 9):lower() == "/default:" then
        default = arg:sub(10)
    elseif prompt == nil then
        prompt = arg
    end
end

function check(b, m)
    if not b then
      print(m)
    end
    return b
end

hConsoleInput = check(w.CreateFile("CONIN$", w.Access.GENERIC_READ, w.FileShare.READ, nil, w.Creation.OPEN_EXISTING), "Error opening CONIN$")
hConsoleOutput = check(w.CreateFile("CONOUT$", w.Access.GENERIC_WRITE, w.FileShare.WRITE, nil, w.Creation.OPEN_EXISTING), "Error opening CONOUT$")

if prompt then
    check(w.WriteConsole(hConsoleOutput, prompt, string.len(prompt)), "Error WriteConsole")
end

if default then
    inputcontrol = {}
    inputcontrol.nInitialChars = string.len(default)
    inputcontrol.dwCtrlWakeupMask = 0
    inputcontrol.dwControlKeyState = 0
    check(w.WriteConsole(hConsoleOutput, default, inputcontrol.nInitialChars), "Error WriteConsole")
end

success, msg = w.ReadConsole(hConsoleInput, 1024, nil, inputcontrol)
check(success, "Error WriteConsole")

w.CloseHandle(hConsoleInput)
w.CloseHandle(hConsoleOutput)

io.stdout:write(msg)
