@echo off
setlocal enabledelayedexpansion

REM cia-to-cci: Convert Nintendo 3DS CIA files to decrypted CCI
REM This script wraps the Docker image so you can use it like a native command.
REM
REM Prerequisites:
REM   - Docker Desktop installed and running
REM   - AES keys file at %USERPROFILE%\.3ds\aes_keys.txt
REM
REM Usage:
REM   cia-to-cci game.cia
REM   cia-to-cci game.cia -o output.cci
REM   cia-to-cci game.cia -o D:\Output\game.cci --keys E:\keys\aes_keys.txt
REM   cia-to-cci --help

set "IMAGE_NAME=cia-to-cci"

REM Build the Docker image if it doesn't exist locally
docker image inspect %IMAGE_NAME% >nul 2>&1
if errorlevel 1 (
    echo Docker image not found. Building %IMAGE_NAME%...
    docker build -t %IMAGE_NAME% https://github.com/reyrodrigues/cia-to-cci.git
    if errorlevel 1 (
        echo Error: Failed to build Docker image.
        exit /b 1
    )
)

REM Handle --help / -h (no files needed)
for %%A in (%*) do (
    if "%%~A"=="--help" ( docker run --rm %IMAGE_NAME% --help & exit /b 0 )
    if "%%~A"=="-h" ( docker run --rm %IMAGE_NAME% --help & exit /b 0 )
)

REM Parse arguments to find file paths
set "INPUT_FILE="
set "OUTPUT_FILE="
set "KEYS_FILE="
set "KEEP_TEMP="
set "NEXT_IS="

for %%A in (%*) do (
    if "!NEXT_IS!"=="output" (
        set "OUTPUT_FILE=%%~fA"
        set "NEXT_IS="
    ) else if "!NEXT_IS!"=="keys" (
        set "KEYS_FILE=%%~fA"
        set "NEXT_IS="
    ) else if "%%~A"=="-o"        ( set "NEXT_IS=output"
    ) else if "%%~A"=="--output"  ( set "NEXT_IS=output"
    ) else if "%%~A"=="--keys"    ( set "NEXT_IS=keys"
    ) else if "%%~A"=="--keep-temp" ( set "KEEP_TEMP=1"
    ) else (
        set "INPUT_FILE=%%~fA"
    )
)

REM Validate input
if not defined INPUT_FILE (
    echo Error: No input file specified.
    echo Usage: cia-to-cci game.cia [-o output.cci] [--keys path\to\keys.txt]
    exit /b 1
)
if not exist "!INPUT_FILE!" (
    echo Error: Input file not found: !INPUT_FILE!
    exit /b 1
)

REM Resolve input path components
for %%F in ("!INPUT_FILE!") do (
    set "INPUT_DIR=%%~dpF"
    set "INPUT_NAME=%%~nxF"
    set "INPUT_STEM=%%~nF"
)

REM Default keys
if not defined KEYS_FILE set "KEYS_FILE=%USERPROFILE%\.3ds\aes_keys.txt"
if not exist "!KEYS_FILE!" (
    echo Error: AES keys file not found at !KEYS_FILE!
    echo Place your aes_keys.txt in %USERPROFILE%\.3ds\
    exit /b 1
)

REM Build docker volumes and container command
set VOLUMES=-v "!KEYS_FILE!:/root/.3ds/aes_keys.txt" -v "!INPUT_DIR!:/mnt/input"
set "CMD=/mnt/input/!INPUT_NAME!"

REM Handle output file
if defined OUTPUT_FILE (
    for %%F in ("!OUTPUT_FILE!") do (
        set "OUTPUT_DIR=%%~dpF"
        set "OUTPUT_NAME=%%~nxF"
    )
    set VOLUMES=!VOLUMES! -v "!OUTPUT_DIR!:/mnt/output"
    set "CMD=!CMD! -o /mnt/output/!OUTPUT_NAME!"
) else (
    REM Default: write .cci next to the input file
    set "CMD=!CMD! -o /mnt/input/!INPUT_STEM!.cci"
)

if defined KEEP_TEMP set "CMD=!CMD! --keep-temp"

docker run --rm !VOLUMES! %IMAGE_NAME% !CMD!
