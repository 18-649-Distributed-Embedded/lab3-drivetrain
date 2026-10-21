#include <zephyr/kernel.h>
#include "hw/l298n.h"
#include "hw/encoders.h"
#include "hw/adc.h"
#include "hw/blinkers.h"
#include "hw/can.h"
#include "sys/drivetrain.h"
#include "sys/blinkers_control.h"
#include "sys/health.h"
#include "sys/uart_parser.h"
#include "core/log.h"

int main(void)
{
    DEBUG_PRINT("=== Lab 3 Drivetrain Node: Initializing ===\n");

    /* --- Hardware layer --- */
    DEBUG_PRINT("Initializing hardware...\n");
    if (l298n_init()    != 0) return -1;
    if (encoders_init() != 0) return -1;
    if (adc_init()      != 0) return -1;
    if (blinkers_init() != 0) return -1;
    if (can_bus_init()  != 0) return -1;

    /* --- System layer (stub inits only — threads added in Part 2) --- */
    DEBUG_PRINT("Initializing system stubs...\n");
    uart_parser_init();
    drivetrain_init();
    blinkers_control_init();
    health_init();

    DEBUG_PRINT("=== Initialization complete. Checkpoint reached. ===\n");

    /* Verify both CAN controllers came up in ERROR-ACTIVE state */
    can_log_state();

    /* ------------------------------------------------------------------
     * Temporary 1 Hz test TX loop — remove once Part 2 threads are added.
     * Sends ID 0x7FF on CAN A and ID 0x7FE on CAN B so candump shows
     * distinct traffic from each controller.
     *
     *  Byte 0-1  uptime_ms [31:16] and [15:0]
     *  Byte 2    sequence counter (0-255, wraps)
     *  Byte 3    bus id  (0xCA = CAN A, 0xCB = CAN B)
     *  Bytes 4-7 0xDEADBEEF marker
     * ------------------------------------------------------------------ */
    DEBUG_PRINT("Starting 1 Hz CAN test TX...\n");
    uint8_t seq = 0;
    while (1) {
        uint32_t uptime = k_uptime_get_32();

        struct can_frame f = {
            .id  = 0x7FF,
            .dlc = 8,
            .data = {
                (uptime >> 24) & 0xFF,
                (uptime >> 16) & 0xFF,
                (uptime >>  8) & 0xFF,
                (uptime      ) & 0xFF,
                seq,
                0xCA,           /* bus A marker */
                0xDE, 0xAD,
            },
        };

        int r = can_send(can_a_dev, &f, K_MSEC(50), NULL, NULL);
        DEBUG_PRINT("[CAN A] TX 0x7FF seq=%u uptime=%ums ret=%d\n",
                    seq, uptime, r);

        f.id      = 0x7FE;
        f.data[5] = 0xCB;       /* bus B marker */
        r = can_send(can_b_dev, &f, K_MSEC(50), NULL, NULL);
        DEBUG_PRINT("[CAN B] TX 0x7FE seq=%u uptime=%ums ret=%d\n",
                    seq, uptime, r);

        seq++;
        k_msleep(1000);
    }
}
