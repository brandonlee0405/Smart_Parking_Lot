#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/portpins.h>
#include <avr/pgmspace.h>

//FreeRTOS include files
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
enum LEDState {INIT,detect} led_state;

void LEDS_Init(){
	led_state = INIT;
}

unsigned char flag = 0;


void LEDS_Tick(){
	//Actions
	unsigned char test = ~(PINB) & 0x01;
	switch(led_state)
	{
		case INIT:
			break;
		
		case detect:
			break;
			
		default:
			break;
	}
	//Transitions
	switch(led_state)
	{
		case INIT:
			led_state = detect;
			break;
		
		case detect:
			if (!test)
			{
				flag = 1;
			}
			else
			{
				flag = 0;
			}
			break;
			
		default:
			break;
	}
}

void LedSecTask()
{
	LEDS_Init();
	for(;;)
	{
		LEDS_Tick();
		vTaskDelay(100);
	}
}


enum LightState {Light_Off,Light_On} state;

void Light_Init(){
	state = Light_Off;
}


void Light_Tick(){
	//Actions
	switch(state)
	{
		case Light_Off:
			PORTD = 0x00;
			break;
			
		case Light_On:
			PORTD = 0x01;
			break;
			
		default:
			break;
	}
	//Transitions
	switch(state)
	{
		case Light_Off:
			if (flag == 1)
			{
				state = Light_On;
			}
			break;
			
		case Light_On:
			if (flag == 0)
			{
				state = Light_Off;
			}
			break;
			
		default:
			break;
	}
}

void LightSecTask()
{
	Light_Init();
	for(;;)
	{
		Light_Tick();
		vTaskDelay(100);
	}
}


void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(LedSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(LightSecTask, (signed portCHAR *)"LightSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
	DDRB = 0x00; PORTB=0xFF;
	DDRD = 0xFF; PORTD = 0x00;
	//Start Tasks
	StartSecPulse(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}