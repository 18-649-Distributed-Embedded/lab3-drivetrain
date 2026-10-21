#ifndef SYS_PID_H
#define SYS_PID_H

#include <stdint.h>

/* Exposed state for telemetry / health logging */
extern volatile int32_t pid_filtered_tps_l;
extern volatile int32_t pid_filtered_tps_r;
extern volatile uint32_t pid_deadline_misses;

/**
 * @brief Start the PID velocity control task and its 10 ms timer.
 *        Reads drive_target_tps from drive.h, writes L298N PWM.
 */
void pid_init(void);

#endif /* SYS_PID_H */

