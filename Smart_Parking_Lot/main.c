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

unsigned char parking1 = 0;
unsigned char parking2 = 0;
unsigned char parking3 = 0;
unsigned char parking4 = 0;
int parkingFull = 0;


// ======================= LED MATRIX ======================

enum MatrixState {mx_INIT,mx_CROSS} mx_state;

void Matrix_Init()
{
	mx_state = mx_INIT;
}

void Matrix_Tick()
{
	switch(mx_state)
	{
		case mx_INIT:
		PORTC = 0xFF;
		PORTB = 0xE0;
		PORTA = 0xF8;
		break;
		
		case mx_CROSS:
		PORTB = 0xE0;
		PORTC = 0xF8;
		PORTA = 0xFF;
		break;
		
		default:
		break;
	}
	switch(mx_state)
	{
		case mx_INIT:
		if (parking1 == 1)
		{
			mx_state = mx_CROSS;
		}
		break;
		
		case mx_CROSS:
		if (parking1 == 0)
		{
			mx_state = mx_INIT;
		}
		break;
		
		default:
		break;
	}
}

void MatrixSecTask()
{
	Matrix_Init();
	for(;;)
	{
		Matrix_Tick();
		vTaskDelay(1);
	}
}


// =========================================================

// ======================= LED MATRIX ======================

enum MatrixState2 {mx2_INIT,mx2_CROSS} mx2_state;

void Matrix2_Init()
{
	mx2_state = mx2_INIT;
}

void Matrix2_Tick()
{
	switch(mx2_state)
	{
		case mx2_INIT:
		PORTC = 0xFF;
		PORTB = 0xE0;
		PORTA = 0x1F;
		break;
		
		case mx2_CROSS:
		PORTC = 0x1F;
		PORTB = 0xE0;
		PORTA = 0xFF;
		break;
		
		default:
		break;
	}
	switch(mx2_state)
	{
		case mx2_INIT:
		if (parking2 == 1)
		{
			mx2_state = mx2_CROSS;
		}
		break;
		
		case mx2_CROSS:
		if (parking2 == 0)
		{
			mx2_state = mx2_INIT;
		}
		break;
		
		default:
		break;
	}
}

void Matrix2SecTask()
{
	Matrix2_Init();
	for(;;)
	{
		Matrix2_Tick();
		vTaskDelay(1);
	}
}


// =========================================================

// ======================= LED MATRIX ======================

enum MatrixState3 {mx3_INIT,mx3_CROSS} mx3_state;

void Matrix3_Init()
{
	mx3_state = mx3_INIT;
}

void Matrix3_Tick()
{
	switch(mx3_state)
	{
		case mx3_INIT:
		PORTC = 0xFF;
		PORTB = 0x07;
		PORTA = 0xF8;
		break;
		
		case mx3_CROSS:
		PORTC = 0xF8;
		PORTB = 0x07;
		PORTA = 0xFF;
		break;
		
		default:
		break;
	}
	switch(mx3_state)
	{
		case mx3_INIT:
		if (parking3 == 1)
		{
			mx3_state = mx3_CROSS;
		}
		break;
		
		case mx3_CROSS:
		if (parking3 == 0)
		{
			mx3_state = mx3_INIT;
		}
		break;
		
		default:
		break;
	}
}

void Matrix3SecTask()
{
	Matrix3_Init();
	for(;;)
	{
		Matrix3_Tick();
		vTaskDelay(1);
	}
}


// =========================================================


// ======================= LED MATRIX ======================

enum MatrixState4 {mx4_INIT,mx4_CROSS} mx4_state;

void Matrix4_Init()
{
	mx4_state = mx4_INIT;
}

void Matrix4_Tick()
{
	switch(mx4_state)
	{
		case mx4_INIT:
		PORTC = 0xFF;
		PORTB = 0x07;
		PORTA = 0x1F;
		break;
		
		case mx4_CROSS:
		PORTC = 0x1F;
		PORTB = 0x07;
		PORTA = 0xFF;
		break;
		
		default:
		break;
	}
	switch(mx4_state)
	{
		case mx4_INIT:
		if (parking4 == 1)
		{
			mx4_state = mx4_CROSS;
		}
		break;
		
		case mx4_CROSS:
		if (parking4 == 0)
		{
			mx4_state = mx4_INIT;
		}
		break;
		
		default:
		break;
	}
}

void Matrix4SecTask()
{
	Matrix4_Init();
	for(;;)
	{
		Matrix4_Tick();
		vTaskDelay(1);
	}
}


// =========================================================



// ===================== IR Sensor LED =====================

enum SensorState {Sensor_INIT,Sensor_Full} ir_state;

void Sensor_Init()
{
	ir_state = Sensor_INIT;
}

void Sensor_Tick()
{
	//Actions
	unsigned char park1 = ~(PIND) & 0x08;
	unsigned char park2 = ~(PIND) & 0x10;
	unsigned char park3 = ~(PIND) & 0x20;
	unsigned char park4 = ~(PIND) & 0x40;
	switch(ir_state)
	{
		case Sensor_INIT:
		
		if (USART_IsSendReady(0))
		{
			USART_Send(0x0F, 0);
		}
		if (park1)
		{
			parking1 = 1;
		}
		else if (!park1)
		{
			parking1 = 0;
		}
		if (park2)
		{
			parking2 = 1;
		}
		else if (!park2)
		{
			parking2 = 0;
		}
		if (park3)
		{
			parking3 = 1;
		}
		else if (!park3)
		{
			parking3 = 0;
		}
		if (park4)
		{
			parking4 = 1;
		}
		else if (!park4)
		{
			parking4 = 0;
		}
		break;
		
		case Sensor_Full:
			if (USART_IsSendReady(0))
			{
				USART_Send(0xFF, 0);
			}
			PORTC =0x00;
			PORTB = 0xFF;
			
		break;
		
		default:
		break;
	}
	//Transitions
	switch(ir_state)
	{
		case Sensor_INIT:
		if (park1 && park2 && park3 && park4)
		{
			ir_state = Sensor_Full;
		}
		break;
		
		case Sensor_Full:
		if (!(park1 && park2 && park3 && park4))
		{
			ir_state = Sensor_INIT;
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


/*

// ================== Motor Detection LED ==================

unsigned char flag = 0;
unsigned char entrance_detected = 0;
unsigned short openTimer = 0;
unsigned char startNew = 0;

enum LEDState {INIT,detect,carPresent,carLeave} led_state;

void LEDS_Init(){
	led_state = INIT;
}

void LEDS_Tick(){
	//Actions
	unsigned char test = ~(PIND) & 0x01;
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
		if (parking1 == 1)
		{
			m_state = m_INIT;
		}
		else
		{
			m_state = First;
		}
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

*/

void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	//xTaskCreate(LedSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(SensorSecTask, (signed portCHAR *)"SensorSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	//xTaskCreate(MotorSecTask, (signed portCHAR *)"MotorSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(MatrixSecTask, (signed portCHAR *)"MatrixSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(Matrix2SecTask, (signed portCHAR *)"Matrix2SecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(Matrix3SecTask, (signed portCHAR *)"Matrix3SecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(Matrix4SecTask, (signed portCHAR *)"Matrix4SecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
	DDRD = 0x00; PORTD=0xFF;
	DDRA = 0xFF; PORTA = 0x00;
	
	// LED MATRIX
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	
	initUSART(0);
	
	//Start Tasks
	StartSecPulse(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}