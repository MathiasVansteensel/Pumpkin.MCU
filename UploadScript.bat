@echo off
setlocal

set ARDUINO_CLI_PATH="E:\Program Files\Arduino CLI\arduino-cli.exe"
set BOARD_NAME="esp8266:esp8266:nodemcu" REM esp8266:esp8266:nodemcu or esp32:esp32:esp32doit-devkit-v1
set FILENAME="Lamp/lamp.ino"

REM List available COM ports
echo Available COM ports:
for /f "tokens=2 delims=," %%A in ('%ARDUINO_CLI_PATH% board list') do (
  echo %%A
)

REM Select a COM port
set /p COM_PORT="Enter the COM port (e.g., COM3): "

REM Compile
%ARDUINO_CLI_PATH% compile -b %BOARD_NAME% %FILENAME%

REM Upload
%ARDUINO_CLI_PATH% upload -p %COM_PORT% -b %BOARD_NAME% %FILENAME%

PAUSE

endlocal