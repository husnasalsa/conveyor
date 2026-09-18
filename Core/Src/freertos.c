/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tim.h"
#include "usart.h"
#include "spi.h"
#include "MFRC522_STM32.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
    IDLE,
	RFID_DETECT,
	WAITING_PICKUP
} RFIDState_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CS_GPIO_Port GPIOC
#define CS_Pin GPIO_PIN_0
#define RESET_GPIO_Port GPIOC
#define RESET_Pin GPIO_PIN_1

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
RFIDState_t RFID_state = IDLE;
uint8_t uid[4];
//uint8_t writeData[16] = "DATA 1 GANTI";
MFRC522_t rfID = {&hspi2, CS_GPIO_Port, CS_Pin, RESET_GPIO_Port, RESET_Pin};
/*struct dataRead {
	int tag_id;
	char nama_barang[25];
	char kategori[25];
	int berat;
	int kode_rak_tujuan;
};*/

int _write(int fd, unsigned char *buf, int len) {
  if (fd == 1 || fd == 2) {                     // stdout or stderr ?
    HAL_UART_Transmit(&huart2, buf, len, 999);  // Print to the UART
  }
  return len;
}
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId myRFIDHandle;
osThreadId myConveyorHandle;
osMutexId accessBarangHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void read_RFID(void const * argument);
void move_conveyor(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* definition and creation of accessBarang */
  osMutexDef(accessBarang);
  accessBarangHandle = osMutexCreate(osMutex(accessBarang));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myRFID */
  osThreadDef(myRFID, read_RFID, osPriorityAboveNormal, 0, 128);
  myRFIDHandle = osThreadCreate(osThread(myRFID), NULL);

  /* definition and creation of myConveyor */
  osThreadDef(myConveyor, move_conveyor, osPriorityHigh, 0, 128);
  myConveyorHandle = osThreadCreate(osThread(myConveyor), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
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

/* USER CODE BEGIN Header_read_RFID */
/**
* @brief Function implementing the myRFID thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_read_RFID */
void read_RFID(void const * argument)
{
  /* USER CODE BEGIN read_RFID */
  /* Infinite loop */
  MFRC522_Init(&rfID);
  //char buf[256] = {0,};
  for(;;)
  {
	  if (waitcardDetect(&rfID) == STATUS_OK){
		  osStatus status = osMutexWait(accessBarangHandle, osWaitForever);
		  if (status == osOK) {
			  RFID_state = RFID_DETECT;
			  osMutexRelease(accessBarangHandle);
		   }
		  if (MFRC522_ReadUid(&rfID, uid) == STATUS_OK){
			  //USER_LOG("CARD ID:%02X %02X %02X %02X", uid[0], uid[1], uid[2], uid[3]);
			  osDelay(1000);
		  }
		  waitcardRemoval(&rfID);
		  osMutexWait(accessBarangHandle, osWaitForever);
		  RFID_state = WAITING_PICKUP;
		  osMutexRelease(accessBarangHandle);
	  }
	  sprintf(buf, "state: %d", RFID_state);
	  HAL_UART_Transmit(&huart2, (const uint8_t*)buf, strlen(buf), 10);
	  osDelay(10);
  }
  /* USER CODE END read_RFID */
}

/* USER CODE BEGIN Header_move_conveyor */
/**
* @brief Function implementing the myConveyor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_move_conveyor */
void move_conveyor(void const * argument)
{
  /* USER CODE BEGIN move_conveyor */
  /* Infinite loop */
  //static int belt_speed = 1000;
  //char buf[256] = {0,};
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);

	// Start PWM Channels
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // RPWM
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2); // LPWM*/
  for(;;)
  {
	if(RFID_state == IDLE) {
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 1000);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
		osDelay(100);
	} else {
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
		osDelay(100);
	}
    osDelay(5);
    if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)){
    	//RFID_state = IDLE;
    }
  }
  /* USER CODE END move_conveyor */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
