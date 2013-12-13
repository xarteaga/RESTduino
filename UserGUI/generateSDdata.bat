::@echo off

:: Copy the file "bootstrap.min.css"
set bs_src = %CD%\css\bootstrap.min.css
set bs_dst = %CD%\sdFiles\bs.css

echo HTTP/1.1 200 OK << %CD%\sdFiles\bs.css 
echo Content-Type: text/css >> %CD%\sdFiles\bs.css 
echo Connection: close >> %CD%\sdFiles\bs.css

::set cssheader=HTTP/1.1 200 OK\nContent-Type: text/css\nConnection: close
::echo %cssheader% >> textfile.txt
