@echo off
setlocal enabledelayedexpansion

REM Change to project root (script is in scripts/)
cd /d "%~dp0\.."
set "PROJECT_ROOT=%cd%"

set "APP_NAME=MaterialViewer"
set "OUTPUT_DIR=%PROJECT_ROOT%\build\%APP_NAME%"
set "BINARY_NAME=materialviewer.exe"
set "ICON_PATH=%PROJECT_ROOT%\src\resources\materialviewer-logo.png"
set "MSYS2_BIN=C:\msys64\ucrt64\bin"

echo Building materialviewer...
cmake -S "%PROJECT_ROOT%" -B "%PROJECT_ROOT%\build" -G "MinGW Makefiles"
if errorlevel 1 goto :error
cmake --build "%PROJECT_ROOT%\build"
if errorlevel 1 goto :error

echo Creating Windows distribution folder...

REM Clean and create output directory
if exist "%OUTPUT_DIR%" rmdir /s /q "%OUTPUT_DIR%"
mkdir "%OUTPUT_DIR%"

REM Copy binary
copy "%PROJECT_ROOT%\build\%BINARY_NAME%" "%OUTPUT_DIR%\"

REM Resources are embedded in the executable
REM Libraries are statically linked
REM Icon is embedded via Windows resource
echo.
echo All resources, libraries, and icon are embedded in the executable.
echo No external files needed!

echo.
echo Created %OUTPUT_DIR%
echo.
echo The folder contains a fully self-contained Windows distribution.
echo The single .exe file can be distributed standalone.
echo.
echo To create a zip:
echo   powershell Compress-Archive -Path "%OUTPUT_DIR%\*" -DestinationPath "%PROJECT_ROOT%\build\MaterialViewer-Windows.zip"
echo.
goto :eof

:error
echo Build failed!
exit /b 1
