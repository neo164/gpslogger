#include <MapleFreeRTOS821.h>

#include "Screens/ScreenManager.h"
#include "Buttons.h"
#include "GPSThread.h"

#include <STM32ADC.h>

STM32ADC myADC(ADC1);
uint8 pin = PA3;

void vLEDFlashTask(void *pvParameters) 
{
	for (;;) 
	{
		vTaskDelay(2000);
		digitalWrite(PC13, LOW);
		vTaskDelay(100);
		digitalWrite(PC13, HIGH);
	}
}


void int_func() {
  Serial.println(myADC.getData());
};

uint8_t value = 0;
bool up = true;
void handler_led(void)
{
	myADC.calibrate();
	myADC.setSampleRate(ADC_SMPR_1_5);
	myADC.attachInterrupt(int_func, ADC_EOC);
	myADC.setPins(&pin, 1);
	myADC.startConversion();

	up ^= 1;
	digitalWrite(PC13, up);
}


// the setup function runs once when you press reset or power the board
void setup()
{
	pinMode(PC13, OUTPUT);
	pinMode(PA3, INPUT_ANALOG);//AD pin.

	Serial.begin(115200);

	initDisplay();
	initButtons();
	initScreens();
	initGPS();
	
	Timer4.setChannel1Mode(TIMER_OUTPUTCOMPARE);
	Timer4.setPeriod(500000); // in microseconds
	Timer4.setCompare1(1);      // overflow might be small
	Timer4.attachCompare1Interrupt(handler_led);


	// Set up threads
	// TODO: Consider encapsulating init and task functions into a class(es)
//	xTaskCreate(vLEDFlashTask, "LED Task",	configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(vDisplayTask, "Display Task", 1024, NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(vButtonsTask, "Buttons Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(vGPSTask, "GPS Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);

	// Run scheduler and all the threads
	vTaskStartScheduler();
}

// Arduino boilerplate function. Do not write any code here, it would not execute.
void loop()
{
}

