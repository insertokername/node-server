@echo off
setlocal

rem Get the directory of the batch file
set "batch_dir=%~dp0"

rem Change the working directory to the batch file's directory
cd /d "%batch_dir%"

rem Run the Python script
start /b "" python -c "import pyautogui; pyautogui.screenshot('ss.jpg')"

rem Restore the original working directory
cd /d "%~dp0"

endlocal