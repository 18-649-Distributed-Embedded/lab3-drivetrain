cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(lab3_drivetrain)

target_sources(app PRIVATE
    src/main.c

    # Pure logic
    src/core/conn_state.c

    # Hardware drivers
    src/hw/l298n.c
    src/hw/encoders.c
    src/hw/adc.c
    src/hw/blinkers.c
    src/hw/can.c

    # System tasks
    src/sys/can_rx.c
    src/sys/relay.c
    src/sys/drive.c
    src/sys/pid.c
    src/sys/blinkers_control.c
    src/sys/hb_tx.c
    src/sys/hb_mon.c
    src/sys/health.c
)

target_include_directories(app PRIVATE src)
