#ifndef SYS_DRIVE_H
#define SYS_DRIVE_H

#include <stdint.h>

/* Shared drive state — written by DRIVE, read by PID */
extern volatile int32_t drive_target_tps;   /* target ticks/sec from throttle */
extern volatile int32_t drive_brake_active; /* nonzero = braking */

/**
 * @brief Start the DRIVE task thread.
 *        Blocks on sem_ctrl; applies throttle/brake to L298N.
 */
void drive_init(void);

#endif /* SYS_DRIVE_H */

