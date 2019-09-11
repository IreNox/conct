@echo off

call %~dp0build.cmd

SET ARDUINO_AVR_DUDE=%ARDUINO_HOME%\hardware\tools\avr\bin\avrdude.exe

cd build
%ARDUINO_AVR_DUDE%  -C%ARDUINO_HOME%\hardware\tools\avr\etc\avrdude.conf -v -patmega328p -carduino -PCOM3 -D -Uflash:w:%ARDUINO_SKETCH%.hex:i 
if %ERRORLEVEL% GEQ 1 exit 1
