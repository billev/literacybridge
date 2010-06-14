@echo off
if not [%1]==[] goto run
echo.
echo.prepTB:   
echo           1. Formats DRIVE as FAT32
echo           2. Creates SERIAL_NUMBER file on drive (pre-pending "LB2_00")
echo           2. Copies DIRECTORY to DRIVE.
echo.
echo Usage:    prepTB ^<DIRECTORY^> ^<DRIVE^> ^<SERIAL_NUMBER^>
echo Example:  prepTB exported-content E: 39
echo.
goto end

:run
@echo on
format %2 /FS:FAT32 /v:TB2
. > %2\tsn.LB2_00%3.tsn
FOR /F %%A IN ('dir %1 /b') DO svn export %1\%%A %2\%%A

rem svn export %1 clean
rem xcopy %1 %2 /E
:end
