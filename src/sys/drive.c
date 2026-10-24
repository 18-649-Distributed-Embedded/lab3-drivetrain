#include "sys/drive.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include "sys/can_rx.h"
#include "core/cmd.h"
#include "hw/l298n.h"
#include "core/log.h"

volatile int32_t drive_target_tps   = 0;
volatile int32_t drive_brake_active = 1;    /* start braked */

#define DRIVE_THREAD_STACK  512
#define DRIVE_THREAD_PRIO   3    /* below MCP2515 driver and relay, above PID */

/*
 * Convert throttle byte (0-255, center=127=idle) to TPS target.
 * Matches original formula: (throttle - 127) * 57.
 */
static inline int32_t throttle_to_tps(uint8_t throttle)
{
    return ((int32_t)throttle - 127) * 57;
}

static void drive_thread_func(void *p1, void *p2, void *p3)
{
    while (1) {
        /* Block until a CTRL frame arrives */
        k_sem_take(&sem_ctrl, K_FOREVER);

        uint32_t val      = (uint32_t)atomic_get(&mailbox_ctrl);
        uint8_t  throttle = (val >> 8) & 0xFF;
        uint8_t  brake    = (val     ) & 0xFF;

        bool any_fault = (atomic_get(&system_safety_flags) != 0);

        if (any_fault || brake != 0) {
            l298n_brake();
            drive_brake_active = 1;
            drive_target_tps   = 0;
        } else {
            drive_brake_active = 0;
            drive_target_tps   = throttle_to_tps(throttle);
        }
    }
}

K_THREAD_STACK_DEFINE(drive_stack, DRIVE_THREAD_STACK);
static struct k_thread drive_thread_data;

void drive_init(void)
{
    l298n_brake();

    k_thread_create(&drive_thread_data, drive_stack,
                    K_THREAD_STACK_SIZEOF(drive_stack),
                    drive_thread_func,
                    NULL, NULL, NULL,
                    DRIVE_THREAD_PRIO, 0, K_NO_WAIT);
    k_thread_name_set(&drive_thread_data, "DRIVE");
}
