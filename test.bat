@echo off
setlocal EnableDelayedExpansion

set CFLAGS=
set CFLAGS=!CFLAGS! -std=c++17
set CFLAGS=!CFLAGS! -ferror-limit=6
set CFLAGS=!CFLAGS! -Werror
set CFLAGS=!CFLAGS! -Wall

set OUT_DIR=%~dp0out\
set EXE_FILE=%OUT_DIR%bin\mtb_tests.exe

md %OUT_DIR% 2>NUL
pushd %OUT_DIR%
zig c++ "%~dp0tests\mtb_tests.cpp" !CFLAGS! -o "%EXE_FILE%" && (
    "%EXE_FILE%" --test-suite-exclude="*_SKIP" --test-case-exclude="*_SKIP" --subcase-exclude="*_SKIP" %*
) || (
    echo ERROR: Unable to run tests - compilation failed.
)
popd
