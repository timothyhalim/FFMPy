@echo off

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
