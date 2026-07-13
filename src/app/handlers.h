#ifndef __HANDLERS
#define __HANDLERS

void vApplicationIdleHook(void);
void vApplicationMallocFailedHook(void);
void vApplicationStackOverflowHook(TaskHandle_t t, char *n);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);

#endif // __HANDLERS
