#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdint.h>
#include "stm32l476xx.h"

#define vPortSVCHandler     SVC_Handler
#define xPortPendSVHandler  PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

/* STM32L476RG - Cortex-M4F, 4 NVIC priority bits */
#define configPRIO_BITS                          4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY  15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
#define configKERNEL_INTERRUPT_PRIORITY   ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

#define configCHECK_HANDLER_INSTALLATION    0
#define configSUPPORT_STATIC_ALLOCATION     0
#define configSUPPORT_DYNAMIC_ALLOCATION    1
#define configCPU_CLOCK_HZ                  ( 4000000UL )
#define configTICK_RATE_HZ                  1000UL
#define configTOTAL_HEAP_SIZE               16384        /* <-- was 4096, too small */
#define configMINIMAL_STACK_SIZE            256          /* <-- was 128, too small for CM4F with FPU */
#define configUSE_PREEMPTION                1
#define configMAX_PRIORITIES                5
#define configUSE_TIMERS                    1
#define configTIMER_TASK_PRIORITY           ( configMAX_PRIORITIES - 1 )
#define configTIMER_TASK_STACK_DEPTH        256
#define configTIMER_QUEUE_LENGTH            10
#define configUSE_IDLE_HOOK                 1
#define configUSE_TICK_HOOK                 0
#define configUSE_MALLOC_FAILED_HOOK        1
#define configCHECK_FOR_STACK_OVERFLOW      2
#define configUSE_MUTEXES                   1
#define configUSE_TASK_NOTIFICATIONS        1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES 1
#define configUSE_16_BIT_TICKS              0
#define configIDLE_SHOULD_YIELD             1
#define configMAX_TASK_NAME_LEN             16
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configENABLE_BACKWARD_COMPATIBILITY 0
#define configUSE_STREAM_BUFFERS 	    1

#define configASSERT( x )   if( ( x ) == 0 ) { __asm volatile("bkpt #0"); for(;;){} }

#define INCLUDE_vTaskDelay          1
#define INCLUDE_vTaskDelete         1
#define INCLUDE_vTaskSuspend        1
#define INCLUDE_xTaskDelayUntil     1
#define INCLUDE_vTaskPrioritySet    1
#define INCLUDE_uxTaskPriorityGet   1
#define INCLUDE_xTaskGetSchedulerState 1
#define INCLUDE_xTaskGetCurrentTaskHandle 1
#define INCLUDE_xTaskResumeFromISR  1

#endif /* FREERTOS_CONFIG_H */
