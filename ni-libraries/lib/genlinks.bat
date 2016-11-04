@echo off
setlocal enabledelayedexpansion

mkdir tmp

for %%F in (lib*.so.*) do IF /i "%%~xF" NEQ ".so" (
	set libout=%%~nF
	del !libout!
	echo OUTPUT_FORMAT^(elf32-littlearm^)> tmp\!libout!
	echo GROUP ^( %%F ^)>> tmp\!libout!
)

REM Do a second round from the temp folder

cd tmp

for %%F in (lib*.so.*) do IF /i "%%~xF" NEQ ".so" (
	set libout=%%~nF
	del !libout!
	echo OUTPUT_FORMAT^(elf32-littlearm^)> !libout!
	echo GROUP ^( %%F ^)>> !libout!
)

cd ..
