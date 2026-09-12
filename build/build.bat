@echo off
setlocal

if not exist bin mkdir bin

g++ -std=c++20 -O3 -Wall -Wextra -pedantic ..\src\main.cpp -o bin\kamutoke.exe
if %ERRORLEVEL% NEQ 0 (
    echo Build failed.
    exit /b %ERRORLEVEL%
)

echo Build successful: bin\kamutoke.exe
