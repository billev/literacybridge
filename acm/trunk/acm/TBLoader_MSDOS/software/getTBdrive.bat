FOR /F "usebackq tokens=1,2,4 delims=|" %%i IN (`usbitcmd l`) DO CALL getTBdrive2.bat %%i "%%j" %%k