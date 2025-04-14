#define Platform "x64"
#define MyAppName "RadLua"
#define MyAppBinary "Lua.exe"
#define MyAppVersion GetStringFileInfo('Bin\Release' + Platform + '\' + MyAppBinary, PRODUCT_VERSION)
#define MyAppVersionShort Copy(StringChange(MyAppVersion, '.', ''), 1, 2)
#define MyAppPublisher "RadSoft"
#define MyAppExt ".lua"

[Setup]
AppName={#MyAppName}
AppId={#MyAppName}{#Platform}
AppVersion={#MyAppVersion}
WizardStyle=modern
DefaultDirName={autopf}\{#MyAppPublisher}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputBaseFilename={#MyAppName}Setup.{#Platform}
SetupIconFile=res\lua.ico
UninstallDisplayIcon={app}\bin\{#MyAppBinary}
ArchitecturesInstallIn64BitMode=win64
Compression=lzma2
SolidCompression=yes
ChangesEnvironment=yes
ChangesAssociations=yes
OutputDir=Bin
DisableWelcomePage=no

[Messages]
SetupWindowTitle=Setup - {#MyAppName} {#MyAppVersion}

[Tasks]
Name: envPath; Description: "Add to PATH variable" 
Name: envPathExt; Description: "Add to PATHEXT variable" 

[Files]
Source: "Bin\Release{#Platform}\Lua.exe"; DestDir: "{app}\bin"
Source: "Bin\Release{#Platform}\Lua{#MyAppVersionShort}.dll"; DestDir: "{app}\bin"
Source: "Bin\Release{#Platform}\lfs.dll"; DestDir: "{app}\bin"

[Icons]
Name: "{group}\Lua"; Filename: "{app}\bin\{#MyAppBinary}"

[Registry]
Root: HKA; Subkey: "Software\Microsoft\Windows\CurrentVersion\App Paths\{#MyAppBinary}"; ValueType: string; ValueName: ""; ValueData: "{app}\bin\{#MyAppBinary}"; Flags: uninsdeletevalue;
Root: HKA; Subkey: "Software\Classes\{#MyAppExt}\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}"; ValueData: ""; Flags: uninsdeletevalue;
Root: HKA; Subkey: "Software\Classes\{#MyAppExt}"; ValueType: string; ValueName: "PerceivedType"; ValueData: "text"; Flags: uninsdeletevalue;
Root: HKA; Subkey: "Software\Classes\{#MyAppName}"; ValueType: string; ValueName: ""; ValueData: "Program {#MyAppName}";  Flags: uninsdeletekey;
Root: HKA; Subkey: "Software\Classes\{#MyAppName}\DefaultIcon"; ValueType: string;  ValueName: ""; ValueData: "{app}\bin\{#MyAppBinary},1";
Root: HKA; Subkey: "Software\Classes\{#MyAppName}\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\{#MyAppBinary}"" ""%1"" %*";
Root: HKA; Subkey: "Software\Classes\Applications\{#MyAppBinary}\SupportedTypes"; ValueType: string; ValueName: "{#MyAppExt}"; ValueData: ""

[Code]

function GetEnvironmentKey() : string;
begin
    if IsAdminInstallMode() then
        result := 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment'
    else
        result := 'Environment';
end;

procedure EnvAddPath(value: string; instlPath: string);
var
    Paths: string;
begin
    { Retrieve current path (use empty string if entry not exists) }
    if not RegQueryStringValue(HKEY_AUTO, GetEnvironmentKey(), value, Paths) then
        Paths := '';

    if Paths = '' then
        Paths := instlPath
    else
    begin
        { Skip if string already found in path }
        if Pos(';' + Uppercase(instlPath) + ';',  ';' + Uppercase(Paths) + ';') > 0 then exit;
        if Pos(';' + Uppercase(instlPath) + '\;', ';' + Uppercase(Paths) + ';') > 0 then exit;

        { Append App Install Path to the end of the path variable }
        Log(Format('Right(Paths, 1): [%s]', [Paths[length(Paths)]]));
        if Paths[length(Paths)] = ';' then
            Paths := Paths + instlPath  { don't double up ';' in env(PATH) }
        else
            Paths := Paths + ';' + instlPath;
    end;

    { Overwrite (or create if missing) path environment variable }
    if RegWriteStringValue(HKEY_AUTO, GetEnvironmentKey(), value, Paths)
    then Log(Format('The [%s] added to PATH: [%s]', [instlPath, Paths]))
    else Log(Format('Error while adding the [%s] to PATH: [%s]', [instlPath, Paths]));
end;

procedure EnvRemovePath(value: string; instlPath: string);
var
    Paths: string;
    P, Offset, DelimLen: Integer;
begin
    { Skip if registry entry not exists }
    if not RegQueryStringValue(HKEY_AUTO, GetEnvironmentKey(), value, Paths) then
        exit;

    { Skip if string not found in path }
    DelimLen := 1;     { Length(';') }
    P := Pos(';' + Uppercase(instlPath) + ';', ';' + Uppercase(Paths) + ';');
    if P = 0 then
    begin
        { perhaps instlPath lives in Paths, but terminated by '\;' }
        DelimLen := 2; { Length('\;') }
        P := Pos(';' + Uppercase(instlPath) + '\;', ';' + Uppercase(Paths) + ';');
        if P = 0 then exit;
    end;

    { Decide where to start string subset in Delete() operation. }
    if P = 1 then
        Offset := 0
    else
        Offset := 1;
    { Update path variable }
    Delete(Paths, P - Offset, Length(instlPath) + DelimLen);

    { Overwrite path environment variable }
    if RegWriteStringValue(HKEY_AUTO, GetEnvironmentKey(), value, Paths)
    then Log(Format('The [%s] removed from PATH: [%s]', [instlPath, Paths]))
    else Log(Format('Error while removing the [%s] from PATH: [%s]', [instlPath, Paths]));
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
    if CurStep = ssPostInstall then
    begin
        if WizardIsTaskSelected('envPath') then
            EnvAddPath('Path', ExpandConstant('{app}') +'\bin')
        else
            EnvRemovePath('Path', ExpandConstant('{app}') +'\bin');
        if WizardIsTaskSelected('envPathExt') then
            EnvAddPath('PATHEXT', '{#MyAppExt}')
        else
            EnvRemovePath('PATHEXT', '{#MyAppExt}');
    end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
    if CurUninstallStep = usPostUninstall then
    begin
        EnvRemovePath('Path', ExpandConstant('{app}') +'\bin');
        EnvRemovePath('PATHEXT', '{#MyAppExt}');
    end;
end;
