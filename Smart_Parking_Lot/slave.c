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
#include "usart_ATmega1284.h"

enum LEDState {INIT,DisplayMsg,WAIT,DisplayFull,WAIT2} led_state;


unsigned char originalMessage[] = "  CAR PARKING     SMART SYSTEM";
unsigned char fullMessage[] = "   PARKING LOT     FULL";
unsigned char parkFull;
void LEDS_Init(){
	led_state = INIT;
}

void LEDS_Tick(){
	//Actions
	switch(led_state){
		case INIT:
		break;

		case DisplayMsg:
			LCD_DisplayString(1, originalMessage);
		break;
		
		case WAIT:
			break;

		case DisplayFull:
			LCD_DisplayString(1, fullMessage);
		break;

		default:
		break;
		
	}
	//Transitions
	switch(led_state){
		case INIT:
			led_state = DisplayMsg;
		break;
		
		case DisplayMsg:
			led_state = WAIT;
		break;
		
		case WAIT:
		 if (USART_HasReceived(0))
		 {
			if (USART_Receive(0) == 0xFF)
			{
				parkFull = USART_Receive(0);
				led_state = DisplayFull;
			}
			 
		 }
			break;
			
		case DisplayFull:
			led_state = WAIT2;
		break;
		
		case WAIT2:
		if(USART_HasReceived(0))
		{
			if (USART_Receive(0) == 0x0F)
			{
				parkFull = USART_Receive(0);
				led_state = DisplayMsg;
			}
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
		vTaskDelay(3);
	}
}

//================== Motor Detection LED ==================

unsigned char flag = 0;
unsigned char entrance_detected = 0;
unsigned short openTimer = 0;
unsigned char startNew = 0;
unsigned char parking1;

enum SENSORState {S_INIT,detect,carPresent,carLeave} sen_state;

void Sensor_Init(){
	sen_state = INIT;
}

void Sensor_Tick(){
	//Actions
	unsigned char test = ~(PIND) & 0x80;
	switch(sen_state)
	{
		case S_INIT:
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
	switch(sen_state)
	{
		case S_INIT:
		sen_state = detect;
		break;
		
		case detect:
		if (test)
		{
			sen_state = carPresent;
		}
		break;
		
		case carPresent:
		if (!test)
		{
			sen_state = carLeave;
		}
		break;
		
		case carLeave:
		if (openTimer < 10)
		{
			openTimer++;
		}
		else
		{
			openTimer = 0;
			startNew = 1;
			sen_state = S_INIT;
		}
		break;
		
		default:
		break;
	}
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



// ===================== Motor Rotation =====================
enum MotorState {m_INIT,First,Second,Third,Fourth,Fifth,Sixth,Seventh,Eighth} m_state;
unsigned short numPhases = (90/5.625) * 64;
unsigned short phaseCounter = 0;
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
		if (parkFull == 0xFF)
		{
			m_state = m_INIT;
		}
		else
		{
			m_state = First;
		}
		break;
		
		
		case First:
		if (parkFull == 0xFF)
		{
			m_state = m_INIT;
		}
		else
		{
			if (flag == 1)
			{
				if (entrance_detected == 1)
				{
					// Waits a couple of seconds until gate starts to open 500
					if (waitCounter < 100)
					{
						PORTC = 0x01;
						waitCounter++;
					}
					else
					{
						if (phaseCounter != numPhases)
						{
							PORTC = 0x00;
							phaseCounter++;
							m_state = Second;
						}
					}
				}
				else
				{
					if (openCounter < 100)
					{
						openCounter++;
					}
					else
					{
						if (phaseCounter > 0)
						{
							phaseCounter--;
							m_state = Eighth;
						}
						if (startNew == 1)
						{
							m_state = m_INIT;
						}
					}
				}
			}
		}
		break;
		
		case Second:
		if (parkFull == 0xFF)
		{
			m_state = m_INIT;
		}
		else
		{
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
					if (phaseCounter > 0)
					{
						phaseCounter--;
						m_state = First;
					}
					if (startNew == 1)
					{
						m_state = m_INIT;
					}
				}
			}
		}
		break;
		
		case Third:
		if (parkFull == 0xFF)
		{
			m_state = m_INIT;
		}
		else
		{
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
					if (phaseCounter > 0)
					{
						phaseCounter--;
						m_state = Second;
					}
					if (startNew == 1)
					{
						m_state = m_INIT;
					}
				}
			}
		}
		break;
		
		case Fourth:
		if (parkFull == 0xFF)
		{
			m_state = m_INIT;
		}
		else
		{
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
					if (phaseCounter > 0)
					{
						phaseCounter--;
						m_state = Third;
					}
					if (startNew == 1)
					{
						m_state = m_INIT;
					}
				}
			}
		}
		break;
		
		case Fifth:
		if (parkFull == 0xFF)
		{
			m_state = m_INIT;
		}
		else
		{
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
					if (phaseCounter > 0)
					{
						phaseCounter--;
						m_state = Fourth;
					}
					if (startNew == 1)
					{
						m_state = m_INIT;
					}
				}
			}
		}
		break;
		
		case Sixth:
		if (parkFull == 0xFF)
		{
			m_state = m_INIT;
		}
		else
		{
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
					if (phaseCounter > 0)
					{
						phaseCounter--;
						m_state = Fifth;
					}
					if (startNew == 1)
					{
						m_state = m_INIT;
					}
				}
			}
		}
		break;
		
		case Seventh:
		if (parkFull == 0xFF)
		{
			m_state = m_INIT;
		}
		else
		{
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
					if (phaseCounter > 0)
					{
						phaseCounter--;
						m_state = Sixth;
					}
					if (startNew == 1)
					{
						m_state = m_INIT;
					}
				}
			}
		}
		break;
		
		case Eighth:
		if (parkFull == 0xFF)
		{
			m_state = m_INIT;
		}
		else
		{
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
					if (phaseCounter > 0)
					{
						phaseCounter--;
						m_state = Seventh;
					}
					if (startNew == 1)
					{
						m_state = m_INIT;
					}
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
	xTaskCreate(SensorSecTask, (signed portCHAR *)"SensorSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(MotorSecTask, (signed portCHAR *)"MotorSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(LedSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00; // LCD DATA LINES
	DDRA = 0xFF; PORTA = 0x00; // LCD CONTROL LINES
	DDRC = 0xFF; PORTC = 0x00; // Stepper Motor
	DDRD = 0x00; PORTD = 0xFF;
	
	
	LCD_init();
	initUSART(0);
	
	//Start Tasks
	StartSecPulse(1);
	//RunSchedular
	vTaskStartScheduler();
	
	
	return 0;
}