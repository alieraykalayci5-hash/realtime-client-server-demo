@echo off
setlocal
cd /d "%~dp0"

start "UDP SERVER" cmd /k ""%~dp0udp_server.exe""
timeout /t 1 >nul
start "UDP CLIENT" cmd /k ""%~dp0udp_client.exe""

endlocal
