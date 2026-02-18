@echo off
setlocal
cd /d "%~dp0"

del /q *.obj 2>nul
del /q *.exe 2>nul

echo [OK] Cleaned.
endlocal
