/**
 * @file nanoRTOS.c
 * @author Ehud Frank
 * Description File containing a nano sized, low power, event driven,
 * real time operating system.
 * @date 25 Nov 2019
 */

#include "nanoRTOS.h"
#include "nanoQueue.h"
#include "string.h"

//
/**
 * A macro to create a queue to hold nOS_task_t elements
 */
nOS_CREATE_TYPED_QUEUE(task_queue, nOS_task_t)
/**
 * This structure joins the data needed for a Task Control Block (TCB)
 */
typedef struct
{
    uint8_t prio_; // The priority of the task
    task_queue_t task_queue_; // A queue structure to queue tasks
} nOS_tcb_t;

/**
 * A structure to hold all the private variables of the module
 */
typedef struct
{
    uint8_t current_prio_;    // The current running priority of the task
    uint8_t read_queue_flags; // A variable to indicate which queue needs to be scheduled
} private_vars_t;

static nOS_task_t prio1_task_q_container_[nOS_PRIO1_TASK_QUEUE_LENGTH];
static nOS_task_t prio2_task_q_container_[nOS_PRIO2_TASK_QUEUE_LENGTH];
static nOS_task_t prio3_task_q_container_[nOS_PRIO3_TASK_QUEUE_LENGTH];
static nOS_task_t prio4_task_q_container_[nOS_PRIO4_TASK_QUEUE_LENGTH];
static nOS_task_t prio5_task_q_container_[nOS_PRIO5_TASK_QUEUE_LENGTH];
static nOS_task_t prio6_task_q_container_[nOS_PRIO6_TASK_QUEUE_LENGTH];
static nOS_task_t prio7_task_q_container_[nOS_PRIO7_TASK_QUEUE_LENGTH];
static nOS_task_t prio8_task_q_container_[nOS_PRIO8_TASK_QUEUE_LENGTH];
// read_queue_flags_mask is an array to allow quick conversion from priority number to bit field mask
static const uint8_t read_queue_flags_mask[8] =
{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
//{ 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

static nOS_tcb_t nOS_tcb_[8];
static private_vars_t prvt_vars;

/**
 * @brief A function to initialise the prio_task_q_containers
 */
static void init_nOS_tcb (void);

nOS_err_t nOS_start (void)
{
    // Initialise the task control block array
    nOS_CRITICAL_SECTION(init_nOS_tcb ()
    ;
    )
    return 0;
}

nOS_err_t nOS_task_enqueue (uint8_t prio, nOS_task_callback_t callback,
                            uint8_t event)
{
    nOS_task_t task;

    // Check inputs to function
    if (NULL == callback)
    {
        return nOS_TASK_ERR;
    }
    if ((prio < 1) || (prio > 8))
    {
        return nOS_PRIORITY_ERR;
    }
    if (task_queue_is_full (&nOS_tcb_[prio - 1].task_queue_))
    {
        return nOS_TASK_QUEUE_ERR;
    }

    task.callback_ = callback;
    task.event_ = event;

    nOS_tcb_[prio - 1].prio_ = prio;
    task_queue_in (&nOS_tcb_[prio - 1].task_queue_, &task);
    prvt_vars.read_queue_flags |= read_queue_flags_mask[prio - 1];

    return nOS_OK;
}

nOS_err_t nOS_schedule (void)
{
    static uint8_t const log2Lkup[] =
    { 0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5,
            5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
            6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8 };

    nOS_task_t task;
    nOS_tcb_t *nOS_tcb;
    uint8_t tcb_index;

    // The scheduler always try to clear the ready task queue flags
    while (prvt_vars.read_queue_flags)
    {
        // Assign a pointer to the highest priority pending queue
        tcb_index = log2Lkup[prvt_vars.read_queue_flags] - 1;
        nOS_tcb = &nOS_tcb_[tcb_index];
        // If this is the last element of the queue
        // , we can clear the pending task queue flag
        if (1 == nOS_tcb->task_queue_.count)
        {
            prvt_vars.read_queue_flags &= ~(read_queue_flags_mask[nOS_tcb->prio_
                    - 1]);
        }
        // Dequeuing the last element
        task_queue_out (&nOS_tcb->task_queue_, &task);
        // Call the task with the event as parameter
        task.callback_ (task.event_);
    }

    return 0;
}

/* ------------------------------------------------------------- */
/* Private function */
/* ------------------------------------------------------------- */
/**
 * @param nOS_INSTALL_QUEUE_APIs(task_queue, nOS_task_t)
 * @param nOS_INSTALL_QUEUE_APIs(task_queue, nOS_task_t)
 * @param nOS_INSTALL_QUEUE_APIs(task_queue, nOS_task_t)
 * @return
 */
/**
 * @brief installing the code for the queue
 * @param nOS_INSTALL_QUEUE_APIs(task_queue, nOS_task_t)
 */
nOS_INSTALL_QUEUE_APIs(task_queue, nOS_task_t)

// function to initialise the TCB's
static void init_nOS_tcb (void)
{
    // Clearing the task queue ready flags and setting priority to 0 (idle)
    memset (&prvt_vars, 0, sizeof(prvt_vars));
    // Clear all the nOS_tcb_
    memset (nOS_tcb_, 0, sizeof(nOS_tcb_));
    // Initialise all TCB by assigning the following:
    // Priority, pointer to the queue data container
    // The user defined queue length
    // priority 1
    nOS_tcb_[0].prio_ = 1;
    nOS_tcb_[0].task_queue_.capacity = nOS_PRIO1_TASK_QUEUE_LENGTH;
    nOS_tcb_[0].task_queue_.values = prio1_task_q_container_;
    // priority 2
    nOS_tcb_[1].prio_ = 2;
    nOS_tcb_[1].task_queue_.capacity = nOS_PRIO2_TASK_QUEUE_LENGTH;
    nOS_tcb_[1].task_queue_.values = prio2_task_q_container_;
    // priority 3
    nOS_tcb_[2].prio_ = 3;
    nOS_tcb_[2].task_queue_.capacity = nOS_PRIO3_TASK_QUEUE_LENGTH;
    nOS_tcb_[2].task_queue_.values = prio3_task_q_container_;
    // priority 4
    nOS_tcb_[3].prio_ = 4;
    nOS_tcb_[3].task_queue_.capacity = nOS_PRIO4_TASK_QUEUE_LENGTH;
    nOS_tcb_[3].task_queue_.values = prio4_task_q_container_;
    // priority 5
    nOS_tcb_[4].prio_ = 5;
    nOS_tcb_[4].task_queue_.capacity = nOS_PRIO5_TASK_QUEUE_LENGTH;
    nOS_tcb_[4].task_queue_.values = prio5_task_q_container_;
    // priority 6
    nOS_tcb_[5].prio_ = 6;
    nOS_tcb_[5].task_queue_.capacity = nOS_PRIO6_TASK_QUEUE_LENGTH;
    nOS_tcb_[5].task_queue_.values = prio6_task_q_container_;
    // priority 7
    nOS_tcb_[6].prio_ = 7;
    nOS_tcb_[6].task_queue_.capacity = nOS_PRIO7_TASK_QUEUE_LENGTH;
    nOS_tcb_[6].task_queue_.values = prio7_task_q_container_;
    // priority 8
    nOS_tcb_[7].prio_ = 8;
    nOS_tcb_[7].task_queue_.capacity = nOS_PRIO8_TASK_QUEUE_LENGTH;
    nOS_tcb_[7].task_queue_.values = prio8_task_q_container_;
}
