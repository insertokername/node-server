@echo off

set TaskName=PYTHON_WATCHDOG-start
set BatchDir=%~dp0
set BatFile=task_client_start.bat

rem Check if the task exists and delete it if it does
schtasks /query /TN "%TaskName%" 2>nul
if %errorlevel% equ 0 (
    schtasks /delete /TN "%TaskName%" /F
)

rem Create a new task
schtasks /create /TN "%TaskName%" /TR "%BatchDir%%BatFile%" /SC ONCE /SD 10/10/2010 /ST 00:00