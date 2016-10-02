@echo off
setlocal

set RepoRoot=%~dp0..

set BuildDir=%RepoRoot%/build/win_msvc_x86-64_debug
if not exist "%BuildDir%" mkdir "%BuildDir%"
pushd "%BuildDir%"
  set OutFilePath=tests.exe
  set CFlags=/nologo /Z7 /MTd /EHsc /I"%RepoRoot%/code" /DMTB_Exceptions=MTB_On
  set TestFiles="%RepoRoot%/tests/test_mtb_common.cpp"^
                "%RepoRoot%/tests/test_mtb_memory.cpp"^
                "%RepoRoot%/tests/test_mtb_slice.cpp"
  cl %CFlags% "%RepoRoot%/tests/main.cpp" %TestFiles% /Fe"%OutFilePath%"

  if "%~1"=="run" "%OutFilePath%"
popd
