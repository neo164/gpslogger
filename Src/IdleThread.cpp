#include <Arduino_FreeRTOS.h>

#include "IdleThread.h"

static const uint8_t periodLen = 9; // 2^periodLen ticks

volatile TickType_t curIdleTicks = 0;
volatile TickType_t lastCountedTick = 0;
volatile TickType_t lastCountedPeriod = 0;
volatile TickType_t lastPeriodIdleValue = 0;
volatile TickType_t minIdleValue = 1 << periodLen;

// configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
// implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
// used by the Idle task.
extern "C" void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
									StackType_t **ppxIdleTaskStackBuffer,
									uint32_t *pulIdleTaskStackSize )
{
	// If the buffers to be provided to the Idle task are declared inside this
	// function then they must be declared static - otherwise they will be allocated on
	// the stack and so not exists after this function exits.
	static StaticTask_t xIdleTaskTCB;
	static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

	// Pass out a pointer to the StaticTask_t structure in which the Idle task's
	// state will be stored.
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	// Pass out the array that will be used as the Idle task's stack.
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	// Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	// Note that, as the array is necessarily of type StackType_t,
	// configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

extern "C" void vApplicationIdleHook( void )
{
	// Process idle tick counter
	volatile TickType_t curTick = xTaskGetTickCount();
	if(curTick != lastCountedTick)
	{
		curIdleTicks++;
		lastCountedTick = curTick;
	}
	
	// Store idle metrics each ~0.5 seconds (512 ticks)
	curTick >>= periodLen;
	if(curTick >  lastCountedPeriod)
	{
		lastPeriodIdleValue = curIdleTicks;
		curIdleTicks = 0;
		lastCountedPeriod = curTick;
		
		// Store the max value
		if(lastPeriodIdleValue < minIdleValue)
			minIdleValue = lastPeriodIdleValue;
	}
}

float getCPULoad()
{
	return 100. - 100. * lastPeriodIdleValue /  (1 << periodLen);
}

float getMaxCPULoad()
{
	return 100. - 100. * minIdleValue /  (1 << periodLen);
}
