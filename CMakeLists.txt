cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(lab3_drivetrain)

target_sources(app PRIVATE
    src/main.c
    src/hw/l298n.c
    src/hw/encoders.c
    src/hw/adc.c
    src/hw/blinkers.c
    src/hw/can.c
    src/sys/uart_parser.c
    src/sys/health.c
    src/sys/drivetrain.c
    src/sys/blinkers_control.c
)

target_include_directories(app PRIVATE src)
