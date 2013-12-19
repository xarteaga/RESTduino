:: Copy the file "bootstrap.min.css"
SET "SRC=%CD%\css\bootstrap.min.css"
SET "DST=%CD%\sdFiles\BS.CSS"
XCOPY /F /V /Y "%SRC%" "%DST%"

:: Copy the file "bootstrap-responsive.min.css"
SET "SRC=%CD%\css\bootstrap-responsive.min.css"
SET "DST=%CD%\sdFiles\BSR.CSS"
XCOPY /F /V /Y "%SRC%" "%DST%"

:: Copy the file "bootstrap.min.js"
SET "SRC=%CD%\js\bootstrap.min.js"
SET "DST=%CD%\sdFiles\BSJS.TXT"
XCOPY /F /V /Y "%SRC%" "%DST%"

:: Copy the file "control.js"
SET "SRC=%CD%\js\control.js"
SET "DST=%CD%\sdFiles\CTRL.TXT"
XCOPY /F /V /Y "%SRC%" "%DST%"

:: Copy the file "jquery.min.js"
SET "SRC=%CD%\js\jquery.min.js"
SET "DST=%CD%\sdFiles\JQUERY.TXT"
XCOPY /F /V /Y "%SRC%" "%DST%"

:: Copy the file "shield.txt"
SET "SRC=%CD%\img\shield.txt"
SET "DST=%CD%\sdFiles\SHIELD.TXT"
XCOPY /F /V /Y "%SRC%" "%DST%"

:: Copy the file "index.html"
SET "SRC=%CD%\index.html"
SET "DST=%CD%\sdFiles\INDEX.TXT"
XCOPY /F /V /Y "%SRC%" "%DST%"
