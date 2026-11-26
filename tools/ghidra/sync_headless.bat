@echo off
rem Headless dsd import + sync: builds the dsd memory blocks from a ROM, then applies
rem symbols, functions and relocations from a dsd config. No GUI, no dialogs.
rem
rem   sync_headless.bat <config.yaml> <rom.nds> [arm9|arm7] [dry] [project]
rem
rem config.yaml is the module rom config (config\ATOJ\arm9\config.yaml), not the
rem extract config (extract\ATOJ\config.yaml) - the rom config is the one with the
rem rom_config field. The project defaults to ATOJHL and is created on first run.
setlocal
if "%~1"=="" (
    echo usage: sync_headless.bat ^<config.yaml^> ^<rom.nds^> [arm9^|arm7] [dry] [project]
    exit /b 1
)
set "ROOT=%~dp0..\.."
set "CONFIG=%~f1"
set "ROM=%~f2"
set "CPU=%~3"
if "%CPU%"=="" set "CPU=arm9"
set "DRY=%~4"
set "PROJ=%~5"
if "%PROJ%"=="" set "PROJ=ATOJHL"
set "TOOLS=M:\ATOJ-tools"
set "JAVA_HOME=%TOOLS%\jdk"
set "PATH=%JAVA_HOME%\bin;%PATH%"
set "HL=%TOOLS%\ghidra11\support\analyzeHeadless.bat"
set "SCRIPTS=%~dp0"
if "%SCRIPTS:~-1%"=="\" set "SCRIPTS=%SCRIPTS:~0,-1%"

if exist "%TOOLS%\proj\%PROJ%.gpr" (
    call "%HL%" %TOOLS%\proj %PROJ% -process "arm9.bin" -noanalysis -scriptPath "%SCRIPTS%" -postScript DsdHeadless.java "%CONFIG%" "%ROM%" %CPU% %DRY%
) else (
    call "%HL%" %TOOLS%\proj %PROJ% -import "%ROOT%\build\ATOJ\build\arm9.bin" -processor ARM:LE:32:v5t -loader BinaryLoader -loader-baseAddr 0x02004000 -noanalysis -scriptPath "%SCRIPTS%" -postScript DsdHeadless.java "%CONFIG%" "%ROM%" %CPU% %DRY%
)
