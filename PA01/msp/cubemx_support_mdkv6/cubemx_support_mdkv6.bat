@echo off
set "script_dir=%~dp0"
set "source_dir=%script_dir%ARM_CM4F"
set "target_dir=%script_dir%..\Middlewares\Third_Party\FreeRTOS\Source\portable\RVDS\ARM_CM4F"

xcopy "%source_dir%\*" "%target_dir%\" /Y /S /R /V

if %errorlevel% neq 0 (
    echo An error occurred during cubemx_support_mdkv6.bat process.
    echo Error level: %errorlevel%
    echo source_dir=%source_dir%
    echo target_dir=%target_dir%
    exit /b %errorlevel%
) else (
    echo Updated ARM_CM4F files successfully.
    exit /b 0
)
