@echo off
chcp 65001

:: 设置成功和失败的颜色
set GREEN=0A
set RED=0C

:: 设置目标路径
set RELEASE_DIR=%~dp0release

:: 尝试执行release脚本
start /wait %~dp0tools\git_release\git_release.sh

:: 检查并创建目标文件夹
if not exist "%RELEASE_DIR%\project\output\obj\" (
    mkdir "%RELEASE_DIR%\project\output\obj"
)
:: 尝试复制文件
copy "%~dp0msp\MDK-ARM\output\obj\project.axf" "%RELEASE_DIR%\project\output\obj\project.axf" 2>nul || (
    color %RED%
    echo 复制失败
    pause
    exit /b
)
copy "%~dp0msp\MDK-ARM\project.uvprojx" "%RELEASE_DIR%\project\project.uvprojx" 2>nul || (
    color %RED%
    echo 复制失败
    pause
    exit /b
)
copy "%~dp0tools\git_release\release.md" "%RELEASE_DIR%\release.md" 2>nul || (
    color %RED%
    echo 复制失败
    pause
    exit /b
)

:: 复制成功
color %GREEN%
echo 复制成功
pause
exit
