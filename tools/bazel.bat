@echo off
:: Delayed expansion stays off for the whole script, explicitly rather than by
:: inheritance, so that a "!" in a path or in a forwarded Bazel argument is never
:: eaten while a line is reparsed.  Nothing below may use "!var!".
setlocal disabledelayedexpansion

:: Bazelisk runs this wrapper instead of Bazel itself on Windows.  It bootstraps
:: a private copy of PortableGit, which supplies both git and bash/sh, and puts
:: that in front of whatever is installed on the machine.  Otherwise the shell
:: and git the build ends up using are whichever ones happen to be on PATH.
::
:: Keeping the rest of the environment out of the build is Bazel's job rather
:: than this script's.  Actions are covered by --incompatible_strict_action_env,
:: and repository rules by --experimental_strict_repo_env, which limits them to
:: PATH, PATHEXT, and the variables named by --repo_env.  Anything exported here
:: for a repository rule has to be listed in .bazelrc as well, or it won't
:: arrive.

:: A space in the profile directory breaks paths downstream, so fall back to the
:: short (8.3) form for those users.  Only for those users: 8.3 names have
:: nothing to do with spaces otherwise, and there is no reason to hand everyone
:: else a mangled path.
if not "%USERPROFILE%"=="%USERPROFILE: =%" (
    for %%I in ("%USERPROFILE%") do set "USERPROFILE=%%~sI"
)
:: 8.3 name generation can be turned off, in which case there is nothing to fall
:: back to and Bazel is going to fail in a way that is hard to connect to this.
if not "%USERPROFILE%"=="%USERPROFILE: =%" (
    echo [Wrapper] Warning: %USERPROFILE% contains a space and has no 8.3 short name. >&2
    echo [Wrapper] Bazel may fail on it.  Enable 8.3 names, or move the profile. >&2
)

set "WRAPPER_CACHE_DIR=%USERPROFILE%\.cache\bazel"

:: 1. Figure out which bazel we are supposed to hand off to.
if defined BAZEL_OVERRIDE (
    echo Actually calling "%BAZEL_OVERRIDE%"
    set "BAZEL_TARGET=%BAZEL_OVERRIDE%"
) else (
    rem Ensure Bazelisk integration.
    if not defined BAZEL_REAL (
        echo Error: This script must be run via Bazelisk on Windows. >&2
        exit /b 1
    )
    set "BAZEL_TARGET=%BAZEL_REAL%"
)

:: 2. The pinned hermetic git.
set "GIT_RELEASE_TAG=v2.55.0.windows.5"
set "GIT_ARCHIVE_NAME=PortableGit-2.55.0.5-64-bit.7z.exe"
set "GIT_EXPECTED_SHA256=5aa8a20f6e9abb2c755f0e73c91c687701a46b309ad84a0ca6509380fa4ae290"

:: Key the cache by the whole pin, tag and checksum both, so that any change to
:: the three lines above installs the new release instead of being short
:: circuited by a git.exe that an older revision of this script left behind.
set "GIT_CACHE_DIR=%WRAPPER_CACHE_DIR%\portable_git\%GIT_RELEASE_TAG%-%GIT_EXPECTED_SHA256:~0,12%"
set "GIT_EXE_PATH=%GIT_CACHE_DIR%\cmd\git.exe"

if exist "%GIT_EXE_PATH%" goto git_ready
call :install_git
if errorlevel 1 exit /b 1
:git_ready

:: 3. Put the pinned toolchain in front of anything installed on the machine.
:: PATH is one of the two variables --experimental_strict_repo_env still lets
:: through, so this is what a repository rule resolves bash, sh, and git to.
set "PATH=%GIT_CACHE_DIR%\cmd;%GIT_CACHE_DIR%\bin;%GIT_CACHE_DIR%\usr\bin;%PATH%"

:: Bind the shell explicitly as well, so that rules_shell and rules_cc do not
:: depend on a PATH lookup for it.  BAZEL_SH is passed through by a --repo_env
:: line in .bazelrc.  git has no equivalent -- PATH is how it gets found.
set "BAZEL_SH=%GIT_CACHE_DIR%\bin\bash.exe"

:: Isolating the pinned git from the developer's ~/.gitconfig belongs in
:: .bazelrc, not here.  Exported from this script it would reach every process
:: Bazel launches, and "bazel run //:copybara" needs the real configuration to
:: find a credential helper and an identity to push with.

:: 4. Execute Bazel.
"%BAZEL_TARGET%" %*
exit /b %ERRORLEVEL%

:: Strip the spaces certutil pads its digest with.  A subroutine because reading
:: a variable assigned in the same block would need delayed expansion.
:set_computed_sha256
set "SHA_LINE=%~1"
set "COMPUTED_SHA256=%SHA_LINE: =%"
exit /b 0

:: Fetch, verify, and install the pinned PortableGit release.
::
:: Everything happens in a staging directory private to this process and is
:: published with a single rename, so that two Bazelisk invocations racing on a
:: cold cache can't corrupt each other's download and a failed install never
:: leaves a half unpacked tree behind for the next invocation to trust.
:install_git
echo [Wrapper] Git %GIT_RELEASE_TAG% is not in the runtime cache. Fetching isolated PortableGit... >&2

set "STAGE_DIR=%WRAPPER_CACHE_DIR%\portable_git\staging_%RANDOM%_%RANDOM%"
if exist "%STAGE_DIR%" rmdir /s /q "%STAGE_DIR%"
mkdir "%STAGE_DIR%" 2>nul
if not exist "%STAGE_DIR%" (
    echo Error: Unable to create the staging directory %STAGE_DIR% >&2
    exit /b 1
)

set "GIT_URL=https://github.com/git-for-windows/git/releases/download/%GIT_RELEASE_TAG%/%GIT_ARCHIVE_NAME%"
echo [Wrapper] Downloading from %GIT_URL% ... >&2

curl -fL --silent --show-error --output "%STAGE_DIR%\git.7z.exe" "%GIT_URL%"
if errorlevel 1 (
    echo Error: Failed to download the hermetic Git toolchain >&2
    rmdir /s /q "%STAGE_DIR%"
    exit /b 1
)

echo [Wrapper] Validating cryptographic payload checksum... >&2
set "COMPUTED_SHA256="
:: certutil prints a header line, the digest, then a completion line.  Take the
:: first line after the header rather than filtering the output by text: a cache
:: path containing "CertUtil" would take the header out along with the
:: completion line, leaving the digest as the line that gets skipped.
for /f "skip=1 delims=" %%A in ('certutil -hashfile "%STAGE_DIR%\git.7z.exe" SHA256') do (
    if not defined COMPUTED_SHA256 call :set_computed_sha256 "%%A"
)

if /i not "%COMPUTED_SHA256%"=="%GIT_EXPECTED_SHA256%" (
    echo. >&2
    echo =============================================================== >&2
    echo SECURITY ERROR: Cryptographic checksum mismatch detected. >&2
    echo Expected: %GIT_EXPECTED_SHA256% >&2
    echo Received: %COMPUTED_SHA256% >&2
    echo =============================================================== >&2
    rmdir /s /q "%STAGE_DIR%"
    exit /b 1
)
echo [Wrapper] Integrity verification successful. SHA256 matches. >&2

echo [Wrapper] Extracting archive package... >&2
"%STAGE_DIR%\git.7z.exe" -y -o"%STAGE_DIR%\portable_git" >nul
set "EXTRACT_STATUS=%errorlevel%"
del /q "%STAGE_DIR%\git.7z.exe" 2>nul

if not "%EXTRACT_STATUS%"=="0" (
    echo Error: Extracting the hermetic Git toolchain failed with status %EXTRACT_STATUS% >&2
    rmdir /s /q "%STAGE_DIR%"
    exit /b 1
)
if not exist "%STAGE_DIR%\portable_git\cmd\git.exe" (
    echo Error: cmd\git.exe is missing from the extracted toolchain >&2
    rmdir /s /q "%STAGE_DIR%"
    exit /b 1
)
if not exist "%STAGE_DIR%\portable_git\bin\bash.exe" (
    echo Error: bin\bash.exe is missing from the extracted toolchain >&2
    rmdir /s /q "%STAGE_DIR%"
    exit /b 1
)

:: Publish with a rename.  If another process won the race then GIT_CACHE_DIR
:: already exists, and move drops our copy inside it rather than replacing it,
:: so undo that and keep theirs -- it passed the same checks ours did.
move "%STAGE_DIR%\portable_git" "%GIT_CACHE_DIR%" >nul 2>&1
if exist "%GIT_CACHE_DIR%\portable_git" rmdir /s /q "%GIT_CACHE_DIR%\portable_git"
rmdir /s /q "%STAGE_DIR%" 2>nul

if not exist "%GIT_EXE_PATH%" (
    echo Error: Failed to install the hermetic Git toolchain into %GIT_CACHE_DIR% >&2
    exit /b 1
)

echo [Wrapper] Isolated Git runtime setup completed successfully. >&2
exit /b 0
