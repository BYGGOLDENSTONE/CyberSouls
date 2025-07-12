@echo off
title CyberSouls Git Upload
color 0A

echo ======================================
echo    CyberSouls GitHub Upload Tool
echo ======================================
echo.

cd /d "D:\Unreal Projects\cybersouls"

echo Git durumu kontrol ediliyor...
git status --short
echo.

set /p CONFIRM="Devam etmek istiyor musunuz? (E/H): "
if /i not "%CONFIRM%"=="E" (
    echo Islem iptal edildi.
    pause
    exit /b
)

echo.
echo Degisiklikler ekleniyor...
git add .

echo.
echo Eklenen dosyalar:
git status --short | more
echo.

set /p COMMIT_MSG="Commit mesaji girin: "
if "%COMMIT_MSG%"=="" set COMMIT_MSG=Update CyberSouls project files

echo.
echo Commit yapiliyor...
git commit -m "%COMMIT_MSG%"

echo.
echo GitHub'a yukleniyor...
echo Bu islem buyuk dosyalar icin zaman alabilir...
git push origin master

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Proje basariyla GitHub'a yuklendi!
    echo Repository: https://github.com/BYGGOLDENSTONE/CyberSouls
) else (
    echo.
    echo Hata olustu! Yukaridaki hata mesajini kontrol edin.
)

echo.
pause