@rem
@rem Copyright (c) 2024-2025 Huawei Device Co., Ltd.
@rem Licensed under the Apache License, Version 2.0 (the "License");
@rem you may not use this file except in compliance with the License.
@rem You may obtain a copy of the License at
@rem
@rem http://www.apache.org/licenses/LICENSE-2.0
@rem
@rem Unless required by applicable law or agreed to in writing, software
@rem distributed under the License is distributed on an "AS IS" BASIS,
@rem WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
@rem See the License for the specific language governing permissions and
@rem limitations under the License.
@rem

@echo off

REM Get the directory of the batch file
set BATCH_DIR=%~dp0

REM Check if virtual environment exists
if exist myenv\scripts\activate (
    echo Activating existing virtual environment...
    call myenv\scripts\activate
) else (
    echo Creating a new virtual environment...
    python -m venv myenv
    call myenv\scripts\activate
)

REM Check if PyInstaller is installed
pip show pyinstaller >nul 2>&1
if errorlevel 1 (
    echo Installing PyInstaller...
    pip install pyinstaller
)

REM Check if OpenCV is installed
pip show opencv-python >nul 2>&1
if errorlevel 1 (
    echo Installing OpenCV...
    pip install opencv-python
)

REM Build the CLI executable
echo Building CLI executable...
pyinstaller --onefile --add-data "allow_button_template.jpeg;." --add-data "usb_fullAutomation_newsigned.hap;." --add-data "usbInfo-default-signed.hap;." cli.py
echo CLI generated successfully

REM Move the CLI executable to the batch file directory
move "dist\cli.exe" "%BATCH_DIR%"

REM Build the GUI executable
echo Building GUI executable...
pyinstaller --onefile --windowed --add-data "allow_button_template.jpeg;." --add-data "usb_fullAutomation_newsigned.hap;." --add-data "usbInfo-default-signed.hap;." gui.py

REM Move the GUI executable to the batch file directory
move "dist\gui.exe" "%BATCH_DIR%"

REM Clean up build, dist directories, and spec files
rmdir /s /q build
rmdir /s /q dist
del gui.spec
del cli.spec

REM Deactivate the virtual environment
deactivate

pause