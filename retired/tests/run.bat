@echo off
setlocal EnableDelayedExpansion

set OUT_DIR=%~dp0..\out\
set TEST_DIR=%~dp0

set CFLAGS=
set CFLAGS=!CFLAGS! -std=c++17
set CFLAGS=!CFLAGS! -ferror-limit=6
set CFLAGS=!CFLAGS! -Werror
set CFLAGS=!CFLAGS! -Wall

md %OUT_DIR% 2> NUL
pushd %OUT_DIR%
zig c++ "%TEST_DIR%main.cpp" "%TEST_DIR%mtb.cpp" "%TEST_DIR%tests.cpp" !CFLAGS! -o "%OUT_DIR%bin\mtb_tests.exe" && (
    set RUN=1
) || (
    set RUN=0
)
popd

if "!RUN!" == "1" (
    "%OUT_DIR%bin\mtb_tests.exe" %*
) else (
    echo ERROR: Unable to run tests - compilation failed.
)
