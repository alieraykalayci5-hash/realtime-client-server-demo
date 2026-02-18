@echo off
setlocal
cd /d "%~dp0"

start "TCP SERVER" cmd /k ""%~dp0server.exe""
timeout /t 1 >nul
start "TCP CLIENT" cmd /k ""%~dp0client.exe""

endlocal
