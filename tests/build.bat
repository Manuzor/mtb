@echo off
setlocal

set RepoRoot=%~dp0..

set BuildDir=%RepoRoot%/build/win_msvc_x86-64_debug
if not exist "%BuildDir%" mkdir "%BuildDir%"
pushd "%BuildDir%"
  set CFlags=/nologo /Z7 /MTd /EHsc /I"%RepoRoot%/code"
  cl %CFlags% "%RepoRoot%/tests/main.cpp" "%RepoRoot%/tests/test_mtb_common.cpp" "%RepoRoot%/tests/test_mtb_memory.cpp" /Fetests.exe
popd
