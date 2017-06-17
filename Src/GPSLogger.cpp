#include <Arduino_FreeRTOS.h>

#include "USBDevice.h"
#include "SerialUSB.h"

#include "Screens/ScreenManager.h"
#include "GPS/GPSThread.h"

#include "BoardInit.h"
#include "LEDThread.h"
#include "ButtonsThread.h"

template<size_t STACK_SIZE, UBaseType_t PRIORITY>
class Thread
{
	// Structure that will hold the TCB of the task being created
	StaticTask_t xTaskTCB;
	// Buffer that the task being created will use as its stack.  Note this is
	// an array of StackType_t variables.  The size of StackType_t is dependent on
	// the RTOS port
	StackType_t xStack[ STACK_SIZE ];

	static void vThreadFunc( void * pvParameters )
	{
		((Thread*)pvParameters)->run();
	}

protected:
	virtual void run() = 0;

public:
	void start()
	{
		xTaskCreateStatic(
			Thread::vThreadFunc,				/* Function that implements the task. */
			"Test Thread",				/* Text name for the task. */
			STACK_SIZE,   /* Number of indexes in the xStack array. */
			( void * )this,				/* Parameter passed into the task. */
			PRIORITY,			/* Priority at which the task is created. */
			xStack,					/* Array to use as the task's stack. */
			&xTaskTCB );				/* Variable to hold the task's data structure. */
	}
};


class TestThread : public Thread<configMINIMAL_STACK_SIZE, tskIDLE_PRIORITY+1>
{
protected:
	virtual void run()
	{
		pinMode(PC13, OUTPUT);

		while(1)
		{
			vTaskDelay(100);
			digitalWrite(PC13, !digitalRead(PC13));
		}
	}
} t;

int main(void)
{
	InitBoard();

	//SerialUSB.begin(115200);
	USBDeviceFS.reenumerate();
	USBDeviceFS.beginCDC();

	//initDisplay();
	//initButtons();
	//initScreens();
	//initGPS();

	// Set up threads
	// TODO: Consider encapsulating init and task functions into a class(es)

	//xTaskCreate(vLEDThread, "LED Thread",	configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
	//xTaskCreate(vDisplayTask, "Display Task", 1024, NULL, tskIDLE_PRIORITY + 2, NULL);
	//xTaskCreate(vButtonsThread, "Buttons Thread", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
	//xTaskCreate(vGPSTask, "GPS Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);

	t.start();

	// Run scheduler and all the threads
	vTaskStartScheduler();

	// Never going to be here
	return 0;
}
