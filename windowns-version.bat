@echo off
chcp 65001 >nul
cd /d "%~dp0"

if not exist "build" (
    cmake -S . -B build -G "MinGW Makefiles"
    if errorlevel 1 (
        pause
        exit /b 1
    )
)

cmake --build build
if errorlevel 1 (
    echo Build that bai!
    pause
    exit /b 1
)

if exist "build\run.exe" (
    "build\run.exe"
) else if exist "build\Debug\run.exe" (
    "build\Debug\run.exe"
)