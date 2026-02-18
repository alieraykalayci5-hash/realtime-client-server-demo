@echo off
setlocal

REM --- Load MSVC environment (VS 2026 Community path) ---
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (
  echo [ERROR] MSVC environment failed to load.
  exit /b 1
)

REM --- Go to repo root (this bat's folder) ---
cd /d "%~dp0"

echo [BUILD] TCP...
cl /nologo /EHsc server.cpp /Fe:server.exe || exit /b 1
cl /nologo /EHsc client.cpp /Fe:client.exe || exit /b 1

echo [BUILD] UDP...
cl /nologo /EHsc udp_server.cpp /Fe:udp_server.exe || exit /b 1
cl /nologo /EHsc udp_client.cpp /Fe:udp_client.exe || exit /b 1

echo.
echo [OK] Build complete.
dir *.exe
endlocal
