@echo off
goto :skip_comment

Example batch script to upload files.

Save as ftp_tool.cmd

ftp_tool.bat HOST USER PASS DIR FILES

HOST : hostname | IP address
USER : username to logon with
PASS : password of USER
DIR  : directory to upload to (. is login folder)
FILES: a space separated list of filenames, wildcards allowed

ftp_tool.bat 192.168.2.20 user secret . file1.png file2.txt files* file5
ftp_tool.bat 192.168.2.20 user secret /A file1.png file2.txt files* file5

:skip_comment

setlocal enabledelayedexpansion

set HOST=%1
set USER=%2
set PASS=%3
set DIR=%4

:: Next is an array of file names starting at parameter 5
shift
shift
shift
shift
set FILES=%1

:add_next_file
shift
if "%1" EQU "" goto :added_files
set FILES=%FILES% %1
goto :add_next_file

:added_files
echo %FILES%

:: Generate a unique temporary file name
set TEMP_FILE=ftp_commands_%RANDOM%_%RANDOM%.txt

:: Create the FTP command file
echo open %HOST% > %TEMP_FILE%
echo %USER%>> %TEMP_FILE%
echo %PASS%>> %TEMP_FILE%
echo mkdir %DIR% >> %TEMP_FILE%
echo cd %DIR% >> %TEMP_FILE%
for %%f in (%FILES%) do (
    for %%i in (%%f) do (
        echo mput %%i >> %TEMP_FILE%
    )
)
echo bye >> %TEMP_FILE%

:: Execute the FTP commands
ftp -i -s:%TEMP_FILE%

:: Clean up the temporary file
del %TEMP_FILE%

endlocal
