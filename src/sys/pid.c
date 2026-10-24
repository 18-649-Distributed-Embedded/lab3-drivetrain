#include "sys/pid.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include "sys/drive.h"
#include "hw/l298n.h"
#include "hw/encoders.h"
#include "core/cmd.h"

/* Exported telemetry */
volatile int32_t  pid_filtered_tps_l  = 0;
volatile int32_t  pid_filtered_tps_r  = 0;
volatile uint32_t pid_deadline_misses = 0;

K_SEM_DEFINE(pid_sem, 0, 1);

static uint32_t left_last  = 0;
static uint16_t right_last = 0;
static volatile int32_t isr_vel_l = 0;
static volatile int32_t isr_vel_r = 0;

static void pid_timer_isr(struct k_timer *t)
{
    uint32_t l_curr = encoders_get_left_ticks();
    uint16_t r_curr = encoders_get_right_ticks();

    isr_vel_l = (int32_t)(l_curr - left_last);
    isr_vel_r = (int16_t)(r_curr - right_last);

    left_last  = l_curr;
    right_last = r_curr;

    if (k_sem_count_get(&pid_sem) > 0) {
        pid_deadline_misses++;
    } else {
        k_sem_give(&pid_sem);
    }
}

K_TIMER_DEFINE(pid_timer, pid_timer_isr, NULL);

#define PID_THREAD_STACK  1024
#define PID_THREAD_PRIO   4

static void pid_thread_func(void *p1, void *p2, void *p3)
{
    int32_t filtered_l = 0, filtered_r = 0;
    int32_t integral_l = 0, integral_r = 0;

    while (1) {
        k_sem_take(&pid_sem, K_FOREVER);

        /* If braking or any fault → stop motors, reset PID state */
        if (drive_brake_active || atomic_get(&system_safety_flags) != 0) {
            l298n_brake();
            integral_l = integral_r = 0;
            filtered_l = filtered_r = 0;
            pid_filtered_tps_l = 0;
            pid_filtered_tps_r = 0;
            continue;
        }

        int32_t target = drive_target_tps;

        /* Instantaneous TPS (encoder delta × 100 = ticks/sec at 10ms period) */
        int32_t inst_l = isr_vel_l * 100;
        int32_t inst_r = isr_vel_r * 100;

        /* Low-pass filter (α = 0.25) */
        filtered_l = (filtered_l * 3 + inst_l) / 4;
        filtered_r = (filtered_r * 3 + inst_r) / 4;

        /* PI controller */
        int32_t err_l = target - filtered_l;
        int32_t err_r = target - filtered_r;

        integral_l += err_l;
        integral_r += err_r;

        /* Anti-windup */
        const int32_t INTEGRAL_MAX = 2048000;
        if (integral_l >  INTEGRAL_MAX) integral_l =  INTEGRAL_MAX;
        if (integral_l < -INTEGRAL_MAX) integral_l = -INTEGRAL_MAX;
        if (integral_r >  INTEGRAL_MAX) integral_r =  INTEGRAL_MAX;
        if (integral_r < -INTEGRAL_MAX) integral_r = -INTEGRAL_MAX;

        /* Kp=0.5, Ki=0.0005 — integer arithmetic */
        int32_t out_l = err_l / 2 + integral_l / 2000;
        int32_t out_r = err_r / 2 + integral_r / 2000;

        if (out_l >  1024) out_l =  1024;
        if (out_l < -1024) out_l = -1024;
        if (out_r >  1024) out_r =  1024;
        if (out_r < -1024) out_r = -1024;

        l298n_set_left((int16_t)out_l);
        l298n_set_right((int16_t)out_r);

        pid_filtered_tps_l = filtered_l;
        pid_filtered_tps_r = filtered_r;
    }
}

K_THREAD_STACK_DEFINE(pid_stack, PID_THREAD_STACK);
static struct k_thread pid_thread_data;

void pid_init(void)
{
    left_last  = encoders_get_left_ticks();
    right_last = encoders_get_right_ticks();

    k_timer_start(&pid_timer, K_MSEC(10), K_MSEC(10));

    k_thread_create(&pid_thread_data, pid_stack,
                    K_THREAD_STACK_SIZEOF(pid_stack),
                    pid_thread_func,
                    NULL, NULL, NULL,
                    PID_THREAD_PRIO, 0, K_NO_WAIT);
    k_thread_name_set(&pid_thread_data, "PID");
}
