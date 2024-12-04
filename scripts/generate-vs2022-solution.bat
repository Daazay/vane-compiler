@echo off
pushd %~dp0\..\
call third-party\premake5\premake5.exe --file="scripts\premake5\premake5.lua" vs2022 
popd
PAUSE