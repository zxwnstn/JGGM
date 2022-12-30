@echo off

cd ../../..

if not exist "./Source/ThirdParty/Imgui/.github" (
    goto SubmoduleInit
) else (
    goto GenerateProject
)

:SubmoduleInit
git submodule init Source/ThirdParty/Imgui
::Add Another
git submodule update --remote --recursive

:GenerateProject
cd Projects/Sandbox
call ..\..\ThirdParty\premake\premake5.exe vs2019