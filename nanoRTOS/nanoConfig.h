/**
 * @file nanoRTOS.h
 * @author Ehud Frank
 * @date 25 Nov 2019
 * @brief User can use this file to configure the nano RTOS per application needs.
 */

#ifndef NANOCONFIG_H_
#define NANOCONFIG_H_
/**
 * User needs to determine the length for each prioritised queue
 */

/**
 * @brief User shall provide the interrupt enable/disable function per port
 * Therefore user shall #include the port header as well
 */
//#include "port_mcu#.h"

#define nOS_INTERRUPTS_LOCK()   //__disable_irq()
#define nOS_INTERRUPTS_UNLOCK() //__enable_irq()

/**
 * @brief A macro to wrap a code section with interrupts disable and enable
 */
#define nOS_CRITICAL_SECTION(CODE_TO_GUARD)\
    {\
        nOS_INTERRUPTS_LOCK();\
        CODE_TO_GUARD\
        nOS_INTERRUPTS_UNLOCK();\
    }

#define nOS_PRIO1_TASK_QUEUE_LENGTH         28
#define nOS_PRIO2_TASK_QUEUE_LENGTH         24
#define nOS_PRIO3_TASK_QUEUE_LENGTH         20
#define nOS_PRIO4_TASK_QUEUE_LENGTH         16
#define nOS_PRIO5_TASK_QUEUE_LENGTH         12
#define nOS_PRIO6_TASK_QUEUE_LENGTH         8
#define nOS_PRIO7_TASK_QUEUE_LENGTH         4
#define nOS_PRIO8_TASK_QUEUE_LENGTH         2

/**
 * Compile time check for task queue definitions
 */
#if nOS_PRIO1_TASK_QUEUE_LENGTH == 0
#error("no task queue for priority 1");
#endif
#if nOS_PRIO2_TASK_QUEUE_LENGTH == 0
#error("no task queue for priority 2");
#endif
#if nOS_PRIO3_TASK_QUEUE_LENGTH == 0
#error("no task queue for priority 3");
#endif
#if nOS_PRIO4_TASK_QUEUE_LENGTH == 0
#error("no task queue for priority 4");
#endif
#if nOS_PRIO5_TASK_QUEUE_LENGTH == 0
#error("no task queue for priority 5");
#endif
#if nOS_PRIO6_TASK_QUEUE_LENGTH == 0
#error("no task queue for priority 6");
#endif
#if nOS_PRIO7_TASK_QUEUE_LENGTH == 0
#error("no task queue for priority 7");
#endif
#if nOS_PRIO8_TASK_QUEUE_LENGTH == 0
#error("no task queue for priority 8");
#endif

#endif // end of NANOCONFIG_H_
