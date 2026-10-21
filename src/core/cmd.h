#ifndef CORE_CMD_H
#define CORE_CMD_H

#include <stdint.h>
#include <stdbool.h>
#include <zephyr/sys/atomic.h>

/*
 * System safety / fault flag bits — stored in system_safety_flags atomic.
 * Any nonzero value keeps the motors braked.
 */
#define SAFETY_FLAG_WATCHDOG_TIMEOUT  0  /* no CTRL for > 150 ms         */
#define SAFETY_FLAG_ESTOP             1  /* brake byte asserted in CTRL   */
#define SAFETY_FLAG_ZONE_FAIL_N1      2  /* HB_MON: N1 gone on both buses */
#define SAFETY_FLAG_ZONE_FAIL_N2      3  /* HB_MON: N2 gone on both buses */
#define SAFETY_FLAG_ZONE_FAIL_SELF    4  /* TEST_BTN single press         */
#define SAFETY_FLAG_PEER_FAULT        5  /* received FAULT from another zone */

/*
 * Bus degraded bits — stored separately in bus_health_flags atomic.
 * Nonzero means at least one peer is missing on that bus.
 */
#define BUS_FLAG_DEG_A  0
#define BUS_FLAG_DEG_B  1

extern atomic_t system_safety_flags;
extern atomic_t bus_health_flags;

#endif /* CORE_CMD_H */
