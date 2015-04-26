#THIS IS VERY TEMPORARY, AND VERY SHITTY
#this was written to work on my virtual machine before there was any installer for gazebo, so if you see this, yell at me
#@Author Peter Mitrano <peter@osrfoundation.org>

cd C:\Users\peter\gz-ws\gazebo\build
call ..\win_addpath.bat Debug
cd gazebo
SET GAZEBO_PLUGINS_PATH=%GAZEBO_PLUGINS_PATH%;"C:\Users\peter\wpilib\plugins"
START "gzserver" ".\gzserver.exe" --verbose "%*"
cd gui
START "gzclient" ".\gzclient.exe" --verbose
