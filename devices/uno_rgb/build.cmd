@echo off

SET ARDUINO_HOME=E:\Misc\arduino-1.8.9
SET PATH= %PATH%;%ARDUINO_HOME%

arduino-builder.exe -compile -hardware %ARDUINO_HOME%/hardware -tools %ARDUINO_HOME%/hardware/tools -tools %ARDUINO_HOME%/tools-builder -libraries ../../libraries -libraries ../../third_party -fqbn arduino:avr:uno -build-path ./build uno_rgb.ino