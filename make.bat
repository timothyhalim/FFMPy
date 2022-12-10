@echo off

SET PATH=%PATH%;%~dp0/ffmpeg_x64/bin;
@REM echo %PATH%

REM Convenience wrapper for CMake commands

setlocal enableextensions enabledelayedexpansion

set PROJECT=FFMPy
set BUILD=Release

set BUILD_DIR=build
set INSTALL_DIR=install

@REM Remove existing build folder
rmdir %BUILD_DIR% /S /Q
rmdir %INSTALL_DIR% /S /Q


cmake -B %BUILD_DIR% && cd %BUILD_DIR% && cmake --build . --target ALL_BUILD --config %BUILD%


@REM TEST
"C:/Program Files (x86)/Microsoft Visual Studio/Shared/Python39_64/python.exe" e:/cpp/FFMPy-cmake/test.py
