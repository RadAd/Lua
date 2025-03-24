#define MyAppName "RadLua"
#define MyAppVersion "1.0"
#define MyAppPublisher "RadSoft"
#define MyAppExt ".lua"

[Setup]
AppName={#MyAppName}
AppVersion={#MyAppVersion}
WizardStyle=modern
DefaultDirName={autopf}\{#MyAppPublisher}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputBaseFilename={#MyAppName}Setup
AppModifyPath="{app}\{#MyAppName}Setup.exe" /modify=1
SetupIconFile=res\lua.ico
UninstallDisplayIcon={app}\{#MyAppName}Setup.exe
ArchitecturesInstallIn64BitMode=win64
Compression=lzma2
SolidCompression=yes
ChangesEnvironment=WizardIsTaskSelected('envPath')
ChangesAssociations=yes
OutputDir=Bin

[Tasks]
Name: envPath; Description: "Add to PATH variable" 
Name: envPathExt; Description: "Add to PATHEXT variable" 

[Files]
Source: "Bin\Releasex64\Lua.exe"; DestDir: "{app}\bin"
Source: "Bin\Releasex64\Lua.dll"; DestDir: "{app}\bin"
Source: "Bin\Releasex64\lfs.dll"; DestDir: "{app}\share"

[Registry]
Root: HKA; Subkey: "Software\Classes\{#MyAppExt}\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}"; ValueData: ""; Flags: uninsdeletevalue;
Root: HKA; Subkey: "Software\Classes\{#MyAppExt}"; ValueType: string; ValueName: "PerceivedType"; ValueData: "text"; Flags: uninsdeletevalue;
Root: HKA; Subkey: "Software\Classes\{#MyAppName}"; ValueType: string; ValueName: ""; ValueData: "Program {#MyAppName}";  Flags: uninsdeletekey;
Root: HKA; Subkey: "Software\Classes\{#MyAppName}\DefaultIcon"; ValueType: string;  ValueName: ""; ValueData: "{app}\bin\Lua.exe,1";
Root: HKA; Subkey: "Software\Classes\{#MyAppName}\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\Lua.exe"" ""%1"" %*";

[Code]
const EnvironmentHKey = HKEY_LOCAL_MACHINE;
const EnvironmentKey = 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment';
{ const EnvironmentHKey = HKEY_CURRENT_USER; }
{ const EnvironmentKey = 'Environment'; }

procedure EnvAddPath(value: string; instlPath: string);
var
    Paths: string;
begin
    { Retrieve current path (use empty string if entry not exists) }
    if not RegQueryStringValue(EnvironmentHKey, EnvironmentKey, value, Paths) then
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
    if RegWriteStringValue(EnvironmentHKey, EnvironmentKey, value, Paths)
    then Log(Format('The [%s] added to PATH: [%s]', [instlPath, Paths]))
    else Log(Format('Error while adding the [%s] to PATH: [%s]', [instlPath, Paths]));
end;

procedure EnvRemovePath(value: string; instlPath: string);
var
    Paths: string;
    P, Offset, DelimLen: Integer;
begin
    { Skip if registry entry not exists }
    if not RegQueryStringValue(EnvironmentHKey, EnvironmentKey, value, Paths) then
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
    if RegWriteStringValue(EnvironmentHKey, EnvironmentKey, value, Paths)
    then Log(Format('The [%s] removed from PATH: [%s]', [instlPath, Paths]))
    else Log(Format('Error while removing the [%s] from PATH: [%s]', [instlPath, Paths]));
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
    if CurStep = ssPostInstall then
    begin
        if WizardIsTaskSelected('envPath') then
            EnvAddPath('Path', ExpandConstant('{app}') +'\bin');
        if WizardIsTaskSelected('envPathExt') then
            EnvAddPath('PATHEXT', '{#MyAppExt}');
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
