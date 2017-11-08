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
		if (test)
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

enum MotorState {First,Second,Third,Fourth,Fifth,Sixth,Seventh,Eighth} m_state;
unsigned short numPhases = (90/5.625) * 64;
unsigned char counter = 0;


void Motor_Init(){
	m_state = First;
}


void Motor_Tick(){
	//Actions
	switch(m_state)
	{
		case First:
			PORTA = 0x01;
			break;
		
		case Second:
			PORTA = 0x03;
			break;
		
		case Third:
			PORTA = 0x02;
			break;
		
		case Fourth:
			PORTA = 0x06;
			break;
		
		case Fifth:
			PORTA = 0x04;
			break;
		
		case Sixth:
			PORTA = 0x0C;
			break;
		
		case Seventh:
			PORTA = 0x08;
			break;
		
		case Eighth:
			PORTA = 0x09;
			break;
		
		default:
			break;
	}
	//Transitions
	switch(m_state)
	{
		case First:
			if (flag == 1)
			{
				if (counter != numPhases)
				{
					counter++;
					m_state = Second;
				}
				counter = 0;
			}
			break;
			
		case Second:
			if (flag == 1)
			{
				if (counter != numPhases)
				{
					counter++;
					m_state = Third;
				}
				counter = 0;
			}
			break;
			
		case Third:
			if (flag == 1)
			{
				if (counter != numPhases)
				{
					counter++;
					m_state = Fourth;
				}
				counter = 0;
			}
			break;
			
		case Fourth:
			if (flag == 1)
			{
				if (counter != numPhases)
				{
					counter++;
					m_state = Fifth;
				}
				counter = 0;
			}
			break;
			
		case Fifth:
			if (flag == 1)
			{
				if (counter != numPhases)
				{
					counter++;
					m_state = Sixth;
				}
				counter = 0;
			}
			break;
			
		case Sixth:
			if (flag == 1)
			{
				if (counter != numPhases)
				{
					counter++;
					m_state = Seventh;
				}
				counter = 0;
			}
			break;
			
		case Seventh:
			if (flag == 1)
			{
				if (counter != numPhases)
				{
					counter++;
					m_state = Eighth;
				}
				counter = 0;
			}
			break;
			
		case Eighth:
			if (flag == 1)
			{
				if (counter != numPhases)
				{
					counter++;
					m_state = First;
				}
				counter = 0;
			}
			break;
		
		
		default:
			break;
	}
}

void MotorSecTask()
{
	Motor_Init();
	for(;;)
	{
		Motor_Tick();
		vTaskDelay(3);
	}
}





void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(LedSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(LightSecTask, (signed portCHAR *)"LightSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(MotorSecTask, (signed portCHAR *)"MotorSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
	DDRB = 0x00; PORTB=0xFF;
	DDRD = 0xFF; PORTD = 0x00;
	DDRA = 0xFF; PORTA = 0x00;
	
	//Start Tasks
	StartSecPulse(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}
