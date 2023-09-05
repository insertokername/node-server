@echo off
setlocal

rem Get the directory of the batch file
set "batch_dir=%~dp0"

rem Change the working directory to the batch file's directory
cd /d "%batch_dir%"/client

rem Run the Python script
start python "client.py"

rem Restore the original working directory
cd /d "%~dp0"

endlocal