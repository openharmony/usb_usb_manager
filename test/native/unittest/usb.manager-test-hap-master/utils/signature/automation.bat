@echo off
setlocal

REM Define paths
set "SIGNATURE_DIR=C:\Users\Administrator\Desktop\signature\signature"
set "SOURCE_HAP_FILE=C:\Users\Administrator\DevEcoStudioProjects\MyApplication2\usb\build\default\outputs\default\usb-default-unsigned.hap"
set "DESTINATION_HAP_FILE=%SIGNATURE_DIR%\usb-default-unsigned.hap"
set "SIGN_PROFILE_SCRIPT=%SIGNATURE_DIR%\sign_profile.bat"
set "SIGN_HAP_SCRIPT=%SIGNATURE_DIR%\sign_hap.bat"
set "HDC_COMMAND=hdc install C:\Users\Administrator\Desktop\signature\signature\usb_test_newsigned.hap"

REM Step 1: Delete all .hap and .p7b files in the directory
echo Deleting .hap and .p7b files in %SIGNATURE_DIR%
del /q "%SIGNATURE_DIR%\*.hap"
del /q "%SIGNATURE_DIR%\*.p7b"

REM Step 2: Check if the .hap file exists and copy it
if exist "%SOURCE_HAP_FILE%" (
    echo File %SOURCE_HAP_FILE% exists. Copying to %SIGNATURE_DIR%
    copy /y "%SOURCE_HAP_FILE%" "%DESTINATION_HAP_FILE%"
) else (
    echo File %SOURCE_HAP_FILE% does not exist. Exiting script.
    exit /b 1
)

REM Step 3: Run sign_profile.bat
echo Running %SIGN_PROFILE_SCRIPT%
call "%SIGN_PROFILE_SCRIPT%"

REM Step 4: Run sign_hap.bat
echo Running %SIGN_HAP_SCRIPT%
call "%SIGN_HAP_SCRIPT%"

REM Step 5: Run hdc install command
echo Running %HDC_COMMAND%
%HDC_COMMAND%

endlocal
pause