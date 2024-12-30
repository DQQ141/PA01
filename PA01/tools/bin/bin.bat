@echo off
chcp 65001 

:: export_resource_usage
"%~dp0..\keil5_bar\Keil5_disp_size_bar_v0.4.exe" "%~dp0..\..\msp\MDK-ARM\output\list\project.map"

:: generating_bin_file
del /q "%~dp0..\..\release\0x90000000_firmware.bin"
"C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe" --bin --output="%~dp0..\..\release\0x90000000_firmware.bin" "%~dp0..\..\msp\MDK-ARM\output\obj\project.axf"

:: firmware.md5
del /q "%~dp0..\..\release\firmware.md5"
certutil -hashfile "%~dp0..\..\release\0x90000000_firmware.bin" MD5 > "%~dp0..\..\release\0x90000000_firmware.md5"
