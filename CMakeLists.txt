cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME PowerSupply)
project(${PROJECT_NAME})

set(${CMAKE_PROJECT_NAME}_SKETCH PowerSupply.ino)

#### Uncomment below additional settings as needed.
# set(${CMAKE_PROJECT_NAME}_BOARD mega)
# set(${CMAKE_PROJECT_NAME}_PORT /dev/ttyACM0)
 set(mega.build.mcu atmega2560)
# set(mega.upload.protocol wiring)
# set(mega.upload.speed 115200)
set(INCLUDE_DIRECTORIES "../libraries/")
generate_arduino_firmware(${CMAKE_PROJECT_NAME})
#add_compile_definitions(__AVR_ATmega328P__)


