@echo off

REM Determine script directory
set SCRIPT_DIR=%~dp0
REM Remove trailing backslash
if "%SCRIPT_DIR:~-1%"=="\" set SCRIPT_DIR=%SCRIPT_DIR:~0,-1%

REM Project layout
set PROJECT_ROOT=%SCRIPT_DIR%\..\..
set BUILD_DIR=%SCRIPT_DIR%\..\release

echo Building for Windows (Release)...

REM Clean previous build if requested
if "%1"=="clean" (
    echo Cleaning previous build...
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
)

REM Configure build directory
echo Configuring build directory...
cmake -S "%PROJECT_ROOT%" -B "%BUILD_DIR%" -DCMAKE_BUILD_TYPE=Release

REM Detect number of CPU cores
for /f "tokens=2 delims==" %%a in ('wmic cpu get NumberOfLogicalProcessors /value ^| find "="') do set CORES=%%a
if "%CORES%"=="" set CORES=1

echo Compiling targets with %CORES% threads...
cmake --build "%BUILD_DIR%" --parallel %CORES%

echo Release build complete! Binaries in: %BUILD_DIR%