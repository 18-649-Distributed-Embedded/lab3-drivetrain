#include "sys/drivetrain.h"
#include <zephyr/kernel.h>
#include "hw/l298n.h"
#include "hw/encoders.h"
#include "core/log.h"

/* Exported state — zeroed until threads are re-added in Part 2 */
volatile uint32_t pid_deadline_misses = 0;
volatile int32_t  dt_target_tps       = 0;
volatile int32_t  dt_filtered_tps_l   = 0;
volatile int32_t  dt_filtered_tps_r   = 0;

void drivetrain_init(void)
{
    /* Safe-state the motors immediately */
    l298n_brake();
    /* Thread and PID timer removed for Lab 3 Part 1 — will be re-added in Part 2 */
}
