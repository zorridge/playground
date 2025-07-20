@echo off
setlocal

rem === Config: path to g++ ===
set GPP=C:\msys64\ucrt64\bin\g++.exe

rem === Check if user provided a .cpp file ===
if "%~1"=="" (
    echo Usage: build.bat [source_file.cpp] [args...]
    exit /b 1
)

rem === Extract filename and arguments ===
set FILE=%~1
set NAME=%~n1
shift

rem === Compile ===
echo Compiling %FILE%...
%GPP% -fdiagnostics-color=always -g "%FILE%" ^
    -pedantic-errors -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Werror ^
    -o "%NAME%.exe"

if %errorlevel% neq 0 (
    echo Build failed.
    exit /b %errorlevel%
)

rem === Run the executable with remaining args ===
echo Build succeeded. Running with arguments: %*
"%NAME%.exe" %*
