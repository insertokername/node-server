@echo off
set "batch_dir=%~dp0"

pushd "%batch_dir%"\..\client
rem pushd "%batch_dir%"\..\internal_batch

start "python-watchdog-client" python "client.py"
rem screenshot-test.exe

popd