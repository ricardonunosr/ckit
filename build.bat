@echo off
IF NOT EXIST build mkdir build
pushd build

set C_FLAGS=-nologo /Wall /Zi 
set INCLUDES=/I ..\src 

REM Build static lib (ckit)
cl /c %C_FLAGS% ../src/mem.c

REM Build example executable
cl /c %C_FLAGS% %INCLUDES% ../examples/example.c
cl %C_FLAGS% example.obj mem.obj 
popd 