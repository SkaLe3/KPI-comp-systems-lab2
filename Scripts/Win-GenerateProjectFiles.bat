@echo off

echo Bulding project files

@echo on
cd ..\CSLab2\
call ..\ThirdParty\premake\premake5.exe vs2022
@echo off

PAUSE
