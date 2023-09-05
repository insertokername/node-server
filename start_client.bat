@echo off
setlocal

set "batch_dir=%~dp0"

pushd "%batch_dir%\internal_batch"

call update_task.bat

popd

schtasks /run /tn PYTHON_AFK-start

endlocal
