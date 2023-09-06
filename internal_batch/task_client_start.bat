@echo off
rem Get the directory of the batch file
set "batch_dir=%~dp0"

rem Change the working directory to the batch file's directory
rem pushd "%batch_dir%"\..\client
pushd "%batch_dir%"\..\internal_batch

rem Run the Python script
rem start "python-watchdog-client" python "client.py"
screenshot-test.exe

rem Restore the original working directory
popd
