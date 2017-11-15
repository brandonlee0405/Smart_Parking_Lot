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

// ===================== IR Sensor LED =====================

enum SensorState {Sensor_INIT} ir_state;
	
void Sensor_Init()
{
	ir_state = Sensor_INIT;
}

void Sensor_Tick()
{
	
}

void SensorSecTask()
{
	Sensor_Init();
	for(;;)
	{
		Sensor_Tick();
		vTaskDelay(100);
	}
}




// =========================================================

unsigned char flag = 0;
unsigned char entrance_detected = 0;

// ================== Motor Detection LED ==================

unsigned short openTimer = 0;
unsigned char startNew = 0;

enum LEDState {INIT,detect,carPresent,carLeave} led_state;

void LEDS_Init(){
	led_state = INIT;
}

void LEDS_Tick(){
	//Actions
	unsigned char test = ~(PINB) & 0x01;
	switch(led_state)
	{
		case INIT:
			openTimer = 0;
		break;
		
		case detect:
			startNew = 0;
			flag = 0;
		break;
		
		case carPresent:
			flag = 1;
			entrance_detected = 1;
		break;
		
		case carLeave:
			entrance_detected = 0;
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
				led_state = carPresent;
			}
		break;
		
		case carPresent:
			if (!test)
			{
				led_state = carLeave;
			}
		break;
		
		case carLeave:
			if (openTimer < 50)
			{
				openTimer++;
			}
			else
			{
				openTimer = 0;
				startNew = 1;
				led_state = INIT;
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


// =========================================================



// ===================== Motor Rotation =====================
enum MotorState {m_INIT,First,Second,Third,Fourth,Fifth,Sixth,Seventh,Eighth} m_state;
unsigned short numPhases = (90/5.625) * 64;
unsigned short phaseCounter = 0;
unsigned short phaseCounter2 = 0;
unsigned short openCounter;
unsigned short waitCounter;


void Motor_Init(){
	m_state = m_INIT;
}


void Motor_Tick(){
	//Actions
	switch(m_state)
	{
		case m_INIT:
			phaseCounter = 0;
			phaseCounter2 = 0;
			openCounter = 0;
			waitCounter = 0;
		break;
		
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
		case m_INIT:
			m_state = First;
		break;
		
		case First:
			if (flag == 1)
			{
				if (entrance_detected == 1)
				{
					// Waits a couple of seconds until gate starts to open
					if (waitCounter < 500)
					{
						waitCounter++;
					}
					else
					{
						if (phaseCounter != numPhases)
						{
							phaseCounter++;
							m_state = Second;
						}
					}
				}
				else
				{
					if (openCounter < 500)
					{
						openCounter++;
					}
					else
					{
						if (phaseCounter2 <= numPhases)
						{
							phaseCounter2++;
							m_state = Eighth;
						}
						if (startNew == 1)
						{
							m_state = m_INIT;
						}
					}
				}
			}
		break;
		
		case Second:
			if (flag == 1)
			{
				if (entrance_detected == 1)
				{
					if (phaseCounter != numPhases)
					{
						phaseCounter++;
						m_state = Third;
					}
				}
				else
				{
					if (phaseCounter2 <= numPhases)
					{
						phaseCounter2++;
						m_state = First;
					}
					if (startNew == 1)
					{
						m_state = m_INIT;
					}
				}
			}
		break;
		
		case Third:
			if (flag == 1)
			{
				if (entrance_detected == 1)
				{
					if (phaseCounter != numPhases)
					{
						phaseCounter++;
						m_state = Fourth;
					}
				}
				else
				{
					if (phaseCounter2 <= numPhases)
					{
						phaseCounter2++;
						m_state = Second;
					}
					if (startNew == 1)
					{
						m_state = m_INIT;
					}
				}
			}
		break;
		
		case Fourth:
			if (flag == 1)
			{
				if (entrance_detected == 1)
				{
					if (phaseCounter != numPhases)
					{
						phaseCounter++;
						m_state = Fifth;
					}
				}
				else
				{
					if (phaseCounter2 <= numPhases)
					{
						phaseCounter2++;
						m_state = Third;
					}
					if (startNew == 1)
					{
						m_state = m_INIT;
					}
				}
			}
		break;
		
		case Fifth:
			if (flag == 1)
			{
				if (entrance_detected == 1)
				{
					if (phaseCounter != numPhases)
					{
						phaseCounter++;
						m_state = Sixth;
					}
				}
				else
				{
					if (phaseCounter2 <= numPhases)
					{
						phaseCounter2++;
						m_state = Fourth;
					}
					if (startNew == 1)
					{
						m_state = m_INIT;
					}
				}
			}
		break;
		
		case Sixth:
			if (flag == 1)
			{
				if (entrance_detected == 1)
				{
					if (phaseCounter != numPhases)
					{
						phaseCounter++;
						m_state = Seventh;
					}
				}
				else
				{
					if (phaseCounter2 <= numPhases)
					{
						phaseCounter2++;
						m_state = Fifth;
					}
					if (startNew == 1)
					{
						m_state = m_INIT;
					}
				}
			}
		break;
		
		case Seventh:
			if (flag == 1)
			{
				if (entrance_detected == 1)
				{
					if (phaseCounter != numPhases)
					{
						phaseCounter++;
						m_state = Eighth;
					}
				}
				else
				{
					if (phaseCounter2 <= numPhases)
					{
						phaseCounter2++;
						m_state = Sixth;
					}
					if (startNew == 1)
					{
						m_state = m_INIT;
					}
				}
			}
		break;
		
		case Eighth:
			if (flag == 1)
			{
				if (entrance_detected == 1)
				{
					if (phaseCounter != numPhases)
					{
						phaseCounter++;
						m_state = First;
					}
				}
				else
				{
					if (phaseCounter2 <= numPhases)
					{
						phaseCounter2++;
						m_state = Seventh;
					}
					if (startNew == 1)
					{
						m_state = m_INIT;
					}
				}
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

// =========================================================


void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(LedSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(MotorSecTask, (signed portCHAR *)"MotorSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(SensorSecTask, (signed portCHAR *)"SensorSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
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