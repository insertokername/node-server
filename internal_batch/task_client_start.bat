@echo off
rem Get the directory of the batch file
set "batch_dir=%~dp0"

rem Change the working directory to the batch file's directory
pushd "%batch_dir%"\..\client

rem Run the Python script
start "python-afk-client" python "client.py"

rem Restore the original working directory
popd
