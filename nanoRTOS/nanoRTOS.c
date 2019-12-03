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
#ifdef TDD_BUILD
#include "stdio.h"
#endif
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

uint8_t const nOS_log2_lkup_table[UCHAR_MAX + 1] =
{ 0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

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
nOS_public_t nOS_public;

/**
 * @brief A function to initialise the prio_task_q_containers
 */
static void init_nOS_tcb (void);
#ifndef TDD_BUILD
/**
 * @brief A function to prioritised dequeuing of tasks
 */
nOS_err_t nOS_schedule (void);
#endif

nOS_err_t nOS_start (void)
{
    nOS_STARTUP();
    // Initialise the task control block array
    nOS_CRITICAL_SECTION(init_nOS_tcb ();
#ifndef TDD_BUILD
        nOS_schedule ();
#endif
                         )
#ifndef TDD_BUILD
        while(1)
        {
            nOS_IDLE();
            nOS_schedule ();
        }
#endif
    return 0;
}

nOS_err_t nOS_task_enqueue (uint8_t prio, nOS_task_callback_t callback,
                            uint8_t event)
{
    nOS_task_t l_task;

    // Check inputs to function
    if (NULL == callback)
    {
        return nOS_TASK_ERR;
    }
    if ((prio < 1) || (prio > 8))
    {
        return nOS_PRIORITY_ERR;
    }

    nOS_INTERRUPTS_LOCK ();
    if (task_queue_is_full (&nOS_tcb_[prio - 1].task_queue_))
    {
        nOS_INTERRUPTS_UNLOCK ();
        return nOS_TASK_QUEUE_ERR;
    }

    l_task.callback_ = callback;
    l_task.event_ = event;

    nOS_tcb_[prio - 1].prio_ = prio;
    task_queue_in (&nOS_tcb_[prio - 1].task_queue_, &l_task);
    if (1 == nOS_tcb_[prio - 1].task_queue_.count)
    {
        nOS_public.read_queue_flags |= read_queue_flags_mask[prio - 1];
#ifndef TDD_BUILD
        nOS_schedule ();
#endif
    }

    nOS_INTERRUPTS_UNLOCK ();
    return nOS_OK;
}

nOS_err_t nOS_schedule (void)
{
    nOS_task_t l_task;
    nOS_tcb_t *lp_tcb;
    uint8_t l_prio;
    uint8_t l_pin;

    l_pin = nOS_public.current_prio_;
    l_prio = nOS_log2_lkup_table[nOS_public.read_queue_flags];
// The scheduler always try to clear the ready task queue flags
    while (l_prio > l_pin)
    {
        // Assign a pointer to the highest priority pending queue
        lp_tcb = &nOS_tcb_[l_prio - 1];

        // If this is the last element of the queue
        // , we can clear the pending task queue flag
        if (1 == lp_tcb->task_queue_.count)
        {
            nOS_public.read_queue_flags &= ~(read_queue_flags_mask[l_prio - 1]);
        }

        nOS_INTERRUPTS_UNLOCK();

        // Dequeuing the last element
        int error;
        if ((error = task_queue_out (&lp_tcb->task_queue_, &l_task))
                == nOS_QUEUE_OK)
        {
            // Call the task with the event as parameter
            l_task.callback_ (l_task.event_);
        }
        else
        {
            nOS_INTERRUPTS_LOCK();
            return nOS_TASK_QUEUE_ERR;
        }

        nOS_INTERRUPTS_LOCK();
        l_prio = nOS_log2_lkup_table[nOS_public.read_queue_flags];
    }
    nOS_public.current_prio_ = l_pin;
    return nOS_OK;
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
    memset (&nOS_public, 0, sizeof(nOS_public));
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
