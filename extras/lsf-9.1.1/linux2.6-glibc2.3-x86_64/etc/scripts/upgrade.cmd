@echo off
rem $Id: upgrade.cmd,v 1.1 2006/06/27 19:31:12 gwen Exp $
rem ----------------------------------------------------------------     
rem      
rem   Cmd.exe shell script of simple upgrade for LSF in NT.
rem     
rem ----------------------------------------------------------------     

setlocal

if "%LSF_ENVDIR%"=="" (
    echo.
    echo Please enter LSF_ENVDIR:
    setuputil readline LSF_ENVDIR && call setvar.tmp.cmd
)
if not exist %LSF_ENVDIR%\lsf.conf (
    echo %LSF_ENVDIR%\lsf.conf does not exist
    goto errend
)
setuputil setenvs %LSF_ENVDIR%\lsf.conf || (
    goto errend
)
call setvar.tmp.cmd

echo.
echo Your configurations related to the upgrade are:
echo LSF_SERVERDIR=%LSF_SERVERDIR%
rem echo LSF_INDEP=%LSF_INDEP%
rem echo LSF_MACHDEP=%LSF_MACHDEP%
echo LSF_ENVDIR=%LSF_ENVDIR%
echo LSF_MANDIR=%LSF_MANDIR%
echo LSF_INCLUDEDIR=%LSF_INCLUDEDIR%
echo LSF_MISC=%LSF_MISC%
echo LSF_BINDIR=%LSF_BINDIR%
echo LSF_LIBDIR=%LSF_LIBDIR%
echo.
echo If you want to change the configurations, please cancel the upgrade, and
echo change them in LSF_ENVDIR\lsf.conf -- %LSF_ENVDIR%\lsf.conf.
echo.
echo Do you want to continue the upgrade? (y/n)
setuputil readstr YESNO && call setvar.tmp.cmd
if "%YESNO%"=="y" goto begin
if "%YESNO%"=="Y" goto begin
echo The upgrade is canceled
goto end

:begin

rem Copy LSF_MACHDEP files
for %%d in (base batch batch_js js) do ( 
    if exist %%d\bin (
        echo.
        echo COMMAND: xcopy %%d\bin %LSF_BINDIR% /s /e /f /k /h
        xcopy %%d\bin %LSF_BINDIR% /s /e /f /k /h || (
            echo Failed to copy %%d\bin to %LSF_BINDIR%
            goto errend
        )
    )
)
for %%d in (base batch batch_js js) do ( 
    if exist %%d\etc (
        echo.
        echo COMMAND: xcopy %%d\etc %LSF_SERVERDIR% /s /e /f /k /h
        xcopy %%d\etc %LSF_SERVERDIR% /s /e /f /k /h || (
            echo Failed to copy %%d\etc to %LSF_SERVERDIR%
            goto errend
        )
    )
)
for %%d in (base batch batch_js js) do ( 
    if exist %%d\lib (
        echo.
        echo COMMAND: xcopy %%d\lib %LSF_LIBDIR% /s /e /f /k /h
        xcopy %%d\lib %LSF_LIBDIR% /s /e /f /k /h || (
            echo Failed to copy %%d\lib to %LSF_LIBDIR%
            goto errend
        )
    )
)

rem Copy LSF_INDEP files
for %%d in (base batch batch_js js) do ( 
    if exist %%d\include (
        echo.
        echo COMMAND: xcopy %%d\include %LSF_INCLUDEDIR% /s /e /f /k /h
        xcopy %%d\include %LSF_INCLUDEDIR% /s /e /f /k /h || (
            echo Failed to copy %%d\include to %LSF_INCLUDEDIR%
            goto errend
        )
    )
)
for %%d in (base batch batch_js js) do ( 
    if exist %%d\man (
        echo.
        echo COMMAND: xcopy %%d\man %LSF_MANDIR% /s /e /f /k /h
        xcopy %%d\man %LSF_MANDIR% /s /e /f /k /h || (
            echo Failed to copy %%d\man to %LSF_MANDIR%
            goto errend
        )
    )
)
for %%d in (base batch batch_js js) do ( 
    if exist %%d\misc (
        echo.
        echo COMMAND: xcopy %%d\misc %LSF_MISC% /s /e /f /k /h
        xcopy %%d\misc %LSF_MISC% /s /e /f /k /h || (
            echo Failed to copy %%d\misc to %LSF_MISC%
            goto errend
        )
    )
)

rem Copy examples files
set src=examples
set des=%LSF_MISC%\examples
if exist %src% mkdir %des%
if exist %src% (
    echo.
    echo COMMAND: xcopy %src% %des% /s /e /f /k /h
    xcopy %src% %des% /s /e /f /k /h || (
        echo Failed to copy %src% to %des%
        goto errend
    )
)

rem Copy scripts files
set src=scripts
set des=%LSF_SERVERDIR%\scripts
if exist %src% mkdir %des%
if exist %src% (
    echo.
    echo COMMAND: xcopy %src% %des% /s /e /f /k /h
    xcopy %src% %des% /s /e /f /k /h || (
        echo Failed to copy %src% to %des%
        goto errend
    )
)

rem Copy some commands that are symlinks on UNIX
copy %LSF_BINDIR%\bkill.exe %LSF_BINDIR%\bstop.exe
copy %LSF_BINDIR%\bkill.exe %LSF_BINDIR%\bresume.exe
copy %LSF_BINDIR%\bkill.exe %LSF_BINDIR%\bdel.exe
copy %LSF_BINDIR%\bkill.exe %LSF_BINDIR%\bchkpnt.exe
copy %LSF_BINDIR%\bcaladd.exe %LSF_BINDIR%\bcaldel.exe
copy %LSF_BINDIR%\bcaladd.exe %LSF_BINDIR%\bcalmod.exe
copy %LSF_BINDIR%\bcaladd.exe %LSF_BINDIR%\bcalhist.exe

goto okend

:errend
echo.
echo Simple update of LSF failed
goto end

:okend
echo.
echo Simple update of LSF succeeded

:end

endlocal



