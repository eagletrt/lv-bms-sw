#ifndef APPLICATION_H
#define APPLICATION_H

#include "fsm.h"
#include "tasks.h"
#include "timebase.h"

#define APPLICATION_HEARTBEAT_PERIOD_MS (500U)  /*!< Toggling period of the heartbeat LED. */
#define APPLICATION_FEEDBACK_PERIOD_MS (10U)    /*!< Sampling period of the digital feedbacks. */
#define APPLICATION_DISCHARGE_PERIOD_MS (5000U) /*!< Dwell on each cell during the discharge sweep. */

enum ApplicationTaskId {
    APPLICATION_TASK_MONITOR,
    APPLICATION_TASK_RELAY,
    APPLICATION_TASK_FEEDBACK,
    APPLICATION_TASK_HEARTBEAT,
    APPLICATION_TASK_STATE,
    APPLICATION_TASK_VERSION,
    APPLICATION_TASK_LIBCAN_VERSION,
    APPLICATION_TASK_CELL_VOLTAGES,
    APPLICATION_TASK_VOLTAGE_INFORMATION,
    APPLICATION_TASK_TEMPERATURES,
    APPLICATION_TASK_DISCHARGE,
    APPLICATION_TASK_COUNT
};

struct ApplicationServices {
    const state_t *state;
    const bool *failed;
    void (*feedback)(void);
    void (*heartbeat)(void);
    void (*discharge)(void);
};

extern struct ApplicationServices application_services;
extern struct TimebaseHandler application_timebase;
extern TaskList application_task_list;

#endif
