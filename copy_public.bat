@echo off
echo Copying public files to build directory...
robocopy "%~dp0public" "%~dp0build\public" /E /NFL /NDL /NJH /NJS
if %errorlevel% leq 1 (
    echo Public files copied successfully!
) else (
    echo Error copying files
)
pause
