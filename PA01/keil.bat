@echo off
chcp 65001

cd "%~dp0msp\MDK-ARM"
start project.uvprojx
exit
