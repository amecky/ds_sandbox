@echo off
rem THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
rem ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
rem THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
rem PARTICULAR PURPOSE.
rem
rem Copyright (c) Microsoft Corporation. All rights reserved.

setlocal
set error=0

rem set FX_PATH=C:\devtools\DirectX_SDK\Utilities\bin\x86
set FX_PATH=C:\devtools\DirectX\Utilities\bin\x86

rem Cube
call :CompileShader%1 Grid Grid vs VS_Main
call :CompileShader%1 Grid Grid ps PS_Main

call :CompileShader%1 Depth Depth vs VS_Main

call :CompileShader%1 Shadow Shadow vs VS_Main
call :CompileShader%1 Shadow Shadow ps PS_Main

call :CompileShader%1 Textured Textured vs VS_Main
call :CompileShader%1 Textured Textured ps PS_Main

call :CompileShader%1 Bump Bump vs VS_Main
call :CompileShader%1 Bump Bump ps PS_Main

call :CompileShader%1 AmbientLightning AmbientLightning vs VS_Main
call :CompileShader%1 AmbientLightning AmbientLightning ps PS_Main

call :CompileShader%1 InstancedAmbient InstancedAmbient vs VS_Main
call :CompileShader%1 InstancedAmbient InstancedAmbient ps PS_Main

call :CompileShader%1 Billboards Billboards vs VS_Main
call :CompileShader%1 Billboards Billboards ps PS_Main

call :CompileShader%1 WarpingGrid WarpingGrid vs VS_Main
call :CompileShader%1 WarpingGrid WarpingGrid ps PS_Main

echo.

if %error% == 0 (
    echo Shaders compiled ok
) else (
    echo There were shader compilation errors!
)

endlocal
exit /b

:CompileShader
set fxc=%FX_PATH%\fxc /nologo %1.hlsl /T%3_4_0 /Zi /Zpc /Qstrip_reflect /Qstrip_debug /E%4 /Fh%1_%4.h /Vn%2_%4
echo.
echo %fxc%
%fxc% || set error=1
exit /b
