@echo off
pushd %~dp0\..\
call third-party\premake5\premake5.exe --file="scripts\premake5\premake5.lua" clean-output
popd
PAUSE