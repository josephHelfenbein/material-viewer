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

REM Copy resources
echo Copying resources...
xcopy /E /I /Q "%PROJECT_ROOT%\src\shaders" "%OUTPUT_DIR%\src\shaders"
xcopy /E /I /Q "%PROJECT_ROOT%\src\environments" "%OUTPUT_DIR%\src\environments"
xcopy /E /I /Q "%PROJECT_ROOT%\src\material" "%OUTPUT_DIR%\src\material"
xcopy /E /I /Q "%PROJECT_ROOT%\src\models" "%OUTPUT_DIR%\src\models"
xcopy /E /I /Q "%PROJECT_ROOT%\src\resources" "%OUTPUT_DIR%\src\resources"
xcopy /E /I /Q "%PROJECT_ROOT%\src\ui" "%OUTPUT_DIR%\src\ui"

REM Copy icon if it exists
if exist "%ICON_PATH%" copy "%ICON_PATH%" "%OUTPUT_DIR%\"

REM Bundle required DLLs from MSYS2
echo Bundling DLLs from MSYS2...

REM Core runtime DLLs
set "DLLS=libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll"

REM App dependencies
set "DLLS=%DLLS% glfw3.dll libfreetype-6.dll libzip.dll libzstd.dll"

REM Transitive dependencies
set "DLLS=%DLLS% libbz2-1.dll libbrotlidec.dll libbrotlicommon.dll libharfbuzz-0.dll libglib-2.0-0.dll libgraphite2.dll libintl-8.dll libiconv-2.dll libpcre2-8-0.dll libpng16-16.dll zlib1.dll liblzma-5.dll"

for %%d in (%DLLS%) do (
    if exist "%MSYS2_BIN%\%%d" (
        echo   Copying %%d
        copy "%MSYS2_BIN%\%%d" "%OUTPUT_DIR%\" >nul
    ) else (
        echo   Warning: %%d not found
    )
)

REM Use ldd equivalent - objdump to find any missing DLLs
echo Checking for additional dependencies...
for /f "tokens=*" %%a in ('"%MSYS2_BIN%\objdump.exe" -p "%OUTPUT_DIR%\%BINARY_NAME%" 2^>nul ^| findstr /i "DLL Name"') do (
    for /f "tokens=3" %%b in ("%%a") do (
        set "dll=%%b"
        if not exist "%OUTPUT_DIR%\!dll!" (
            if exist "%MSYS2_BIN%\!dll!" (
                echo   Copying !dll!
                copy "%MSYS2_BIN%\!dll!" "%OUTPUT_DIR%\" >nul
            )
        )
    )
)

echo.
echo Created %OUTPUT_DIR%
echo.
echo The folder contains a fully self-contained Windows distribution.
echo You can zip it for distribution:
echo   powershell Compress-Archive -Path "%OUTPUT_DIR%\*" -DestinationPath "%PROJECT_ROOT%\build\MaterialViewer-Windows.zip"
echo.
goto :eof

:error
echo Build failed!
exit /b 1
