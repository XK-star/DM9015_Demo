/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "main.h"
#include "task.h"
#include "cmsis_os.h"
#include "DM9015.h"

/* USER CODE BEGIN Includes */     

/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId CANSend_TaskHandle;
osThreadId Beep_TaskHandle;
osThreadId Led_ToggleHandle;
osThreadId DM_SendPositionHandle;

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void StartCANSend_Task(void const * argument);
void StartBeep_Task(void const * argument);
void StartDM_SendPosition(void const * argument);
void Led_Toggle_Task(void const * argument);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of CANSend_Task */
//  osThreadDef(CANSend_Task, StartCANSend_Task, osPriorityIdle, 0, 256);
//  CANSend_TaskHandle = osThreadCreate(osThread(CANSend_Task), NULL);

//  /* definition and creation of Beep_Task */
//  osThreadDef(Beep_Task, StartBeep_Task, osPriorityIdle, 0, 256);
//  Beep_TaskHandle = osThreadCreate(osThread(Beep_Task), NULL);

  osThreadDef(Led_Toggle, Led_Toggle_Task, osPriorityNormal, 0, 128);
  Led_ToggleHandle = osThreadCreate(osThread(Led_Toggle), NULL);

//  /* USER CODE BEGIN RTOS_THREADS */
  osThreadDef(DM_SendPosition, StartDM_SendPosition, osPriorityIdle, 0, 256);
  Beep_TaskHandle = osThreadCreate(osThread(DM_SendPosition), NULL);

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* StartCANSend_Task function */
void StartCANSend_Task(void const * argument)
{
  /* USER CODE BEGIN StartCANSend_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartCANSend_Task */
}

/* StartBeep_Task function */
void StartBeep_Task(void const * argument)
{
  /* USER CODE BEGIN StartBeep_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartBeep_Task */
}
void StartDM_SendPosition(void const * argument)
{
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

  for(;;)
  {
		DM_SendPower(256);
		vTaskDelayUntil(&xLastWakeTime,1/portTICK_RATE_MS);
  }
}

/* USER CODE BEGIN Application */
void Led_Toggle_Task(void const * argument)
{

  /* USER CODE BEGIN Led_Toggle_Task */
	
		
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

  /* Infinite loop */
    while(1)
		{
		HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_14);
		vTaskDelayUntil(&xLastWakeTime,100/portTICK_RATE_MS);
		}
	//	vTaskDelayUntil(&xLastWakeTime,100/portTICK_RATE_MS);

  /* USER CODE END Led_Toggle_Task */
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
