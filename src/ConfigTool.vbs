Option Explicit

Dim fso
Dim shell
Dim scriptDir
Dim command
Dim i
Dim arg

Set fso = CreateObject("Scripting.FileSystemObject")
scriptDir = fso.GetParentFolderName(WScript.ScriptFullName)

command = """" & fso.BuildPath(scriptDir, "Player.exe") & """ --config-tool"
For i = 0 To WScript.Arguments.Count - 1
    arg = Replace(WScript.Arguments(i), """", """""")
    command = command & " """ & arg & """"
Next

Set shell = CreateObject("WScript.Shell")
shell.CurrentDirectory = scriptDir
WScript.Quit shell.Run(command, 1, True)
