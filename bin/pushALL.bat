@setlocal enableextensions enabledelayedexpansion
@echo off
set branch=master
set SubScript=lolololol.bat
set Remotes=Temp.filez

findstr "[remote" .git\config > %Remotes%
IF EXIST %SubScript% del /F %SubScript%
echo cls >> %SubScript%
echo @echo off >> %SubScript%
echo echo  ____                   __        ______  __       __         >> %SubScript%
echo echo /\  _ \                /\ \      /\  _  \/\ \     /\ \        >> %SubScript%
echo echo \ \ \_\ \__  __    ____\ \ \____ \ \ \_\ \ \ \    \ \ \       >> %SubScript%
echo echo  \ \  __/\ \/\ \  /  __\\ \  __ \ \ \  __ \ \ \    \ \ \  	>> %SubScript%
echo echo   \ \ \/\ \ \_\ \/\__,  \\ \ \ \ \ \ \ \ \ \ \ \___ \ \ \____ >> %SubScript%
echo echo    \ \_\ \ \____/\/\____/ \ \_\ \_\ \ \_\ \_\ \_____\\ \_____\>> %SubScript%
echo echo     \/_/  \/___/  \/___/   \/_/\/_/  \/_/\/_/\/_____/ \/_____/>> %SubScript%
echo echo.																>>%SubScript%

for /F "tokens=2 delims= " %%A in (%Remotes%) do (
	set ShortVar=%%A
	set ShortVar=!ShortVar:~1,-2!
	echo echo Pushing Repo to: !ShortVar!>> %SubScript%
	echo git push !ShortVar! %branch%>> %SubScript%
	echo echo.>>%SubScript%
)
echo del /f /q %SubScript% >> %SubScript%

for /f %%i in ("%Remotes%") do set size=%%~zi

IF %size% GTR 0 (
	del /f /q %Remotes%
	call %SubScript%
) ELSE (
	cls
	echo  ____                   __        ______  __       __         
	echo /\  _ \                /\ \      /\  _  \/\ \     /\ \        
	echo \ \ \_\ \__  __    ____\ \ \____ \ \ \_\ \ \ \    \ \ \       
	echo  \ \  __/\ \/\ \  /  __\\ \  __ \ \ \  __ \ \ \    \ \ \  	
	echo   \ \ \/\ \ \_\ \/\__,  \\ \ \ \ \ \ \ \ \ \ \ \___ \ \ \____ 
	echo    \ \_\ \ \____/\/\____/ \ \_\ \_\ \ \_\ \_\ \_____\\ \_____\
	echo     \/_/  \/___/  \/___/   \/_/\/_/  \/_/\/_/\/_____/ \/_____/
	echo.																
	echo [ERROR] No Remotes Found! Please check your .git\config file...
	del /f /q %Remotes%
	del /f /q %SubScript%
)
