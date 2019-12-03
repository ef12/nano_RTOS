/**
 * @file nanoRTOS.h
 * @author Ehud Frank
 * @date 25 Nov 2019
 * @brief File containing a nano sized, low power, event driven, real time operating system.
 */

#ifndef NANORTOS_H_
#define NANORTOS_H_

#include <stdint.h>
#include <limits.h>

#include "nanoConfig.h"

/**
 * @brief nOS errors for the nanoRTOS
 */
typedef enum
{
    nOS_OK,             //!< nOS_OK
    nOS_TASK_ERR,       //!< nOS_TASK_ERR
    nOS_PRIORITY_ERR,   //!< nOS_PRIORITY_ERR
    nOS_TASK_QUEUE_ERR, //!< nOS_QUEUE_ERR
    nOS_UNKNOWN_ERR     //!< nOS_UNKNOWN_ERR
} nOS_err_t;

/**
 *
 * @param event
 */
typedef void (*nOS_task_callback_t) (uint8_t event);
/**
 *
 */
typedef struct
{
    nOS_task_callback_t callback_;
    uint8_t event_;
} nOS_task_t;

/**
 * A structure to hold all the private variables of the module
 */
typedef struct
{
    uint8_t current_prio_;    // The current running priority of the task
    uint8_t read_queue_flags; // A variable to indicate which queue needs to be scheduled
} nOS_public_t;

extern nOS_public_t nOS_public;
extern uint8_t const nOS_log2_lkup_table[UCHAR_MAX + 1];
/**
 * @brief nOS_start will start the RTOS by initialising the tasks queues
 * The function shall be called prior to use of the nanoRTOS.
 * @return error code
 */
nOS_err_t nOS_start (void);
/**
 * @brief A function to enqueue a task in on of the priority queues
 * @param prio- The priority of the task
 * @param callback- The actual task callback function
 * @param event- An optional event argument to pass the task per callback
 * @return nOS_err_t
 * @note This function invokes the scheduler.
 */
nOS_err_t nOS_task_enqueue (uint8_t prio, nOS_task_callback_t callback,
                            uint8_t event);

#ifdef TDD_BUILD
/**
 * @brief A function to dequeue all the priority task queues
 * @return nOS_err_t
 */
nOS_err_t nOS_schedule (void);
#endif

#endif /* NANORTOS_H_ */
