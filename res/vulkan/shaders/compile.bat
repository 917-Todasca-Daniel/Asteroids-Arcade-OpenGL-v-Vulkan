@echo off
set COMPILER_PATH=D:/licenta/vulkan/Bin/glslc.exe

set FILENAME=%1

if "%FILENAME%"=="" (
    echo Error: Filename not provided.
    exit /b 1
)

set FILE1=%FILENAME%.vert
set FILE2=%FILENAME%.frag

%COMPILER_PATH% %FILE1% -o v-%FILENAME%.spv
%COMPILER_PATH% %FILE2% -o f-%FILENAME%.spv

pause