#ifndef SYS_CAN_RX_H
#define SYS_CAN_RX_H

#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <stdint.h>

/* Semaphores signaled when fresh data is written to a mailbox */
extern struct k_sem sem_ctrl;    /* given on CTRL (0x201) */
extern struct k_sem sem_blink;   /* given on BLINK (0x200) */

/*
 * Mailboxes (atomic_t, latest-value, non-blocking read)
 *
 * mailbox_ctrl:    [0:8][buttons:8][throttle:8][brake:8]
 * mailbox_blinker: [0:16][blink_state:8][phase_bit:8]
 *
 * HB timestamps: k_uptime_get_32() value at last valid HB reception.
 * Tracked PER BUS before deduplication so HB_MON can detect when a peer
 * drops off one bus while still alive on the other (bus degraded).
 */
extern atomic_t mailbox_ctrl;
extern atomic_t mailbox_blinker;
extern atomic_t mailbox_hb_n1_a;   /* last HB_N1 on bus A */
extern atomic_t mailbox_hb_n1_b;   /* last HB_N1 on bus B */
extern atomic_t mailbox_hb_n2_a;   /* last HB_N2 on bus A */
extern atomic_t mailbox_hb_n2_b;   /* last HB_N2 on bus B */
extern atomic_t mailbox_n1_state;  /* last known zone state of N1 */
extern atomic_t mailbox_n2_state;  /* last known zone state of N2 */

/* STAT line: raw frames received per bus (before dedup) */
extern atomic_t can_rx_count_a;
extern atomic_t can_rx_count_b;

/* Watchdog: last valid CTRL reception time */
extern volatile uint32_t last_ctrl_time;

/**
 * @brief Register RX filter for all incoming frames on both CAN buses.
 */
void can_rx_init(void);

#endif /* SYS_CAN_RX_H */
