@echo off
chcp 65001

cd %~dp0
start git clean -xdf
exit
