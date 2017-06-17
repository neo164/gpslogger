#ifndef LEDDRIVER_H
#define LEDDRIVER_H

#include "Arduino_FreeRTOS.h"

class LEDThread
{
protected:
	static constexpr uint32_t THREAD_STACK_SIZE = configMINIMAL_STACK_SIZE;
	static constexpr UBaseType_t THREAD_PRIORITY = tskIDLE_PRIORITY + 2;
	static constexpr const char * THREAD_NAME = "LED Thread";

protected:
	void run();
};


#endif // LEDDRIVER_H
