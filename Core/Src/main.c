/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static CAN_TxHeaderTypeDef TxMessage;
static CAN_RxHeaderTypeDef RxMessage;

//Rx Txdata
uint8_t TxData[8]={0};
uint32_t TxMailbox;
uint8_t RxData[8]={0};

//address
uint32_t chargerID = 0x1806E8F4;
uint32_t chargerSentID = 0x18FF50E8;
uint32_t receivedID;
uint16_t voltageControl = 4000;
uint16_t currentControl = 100;
uint16_t voltageOutput;
uint16_t currentOutput;
uint8_t control =0;
uint16_t status;

uint16_t cycle = 0;
int error =0 ;
int flag = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void CAN_Txsetup();
void CAN_filterConfig(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */
	CAN_filterConfig();
	CAN_Txsetup();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		HAL_CAN_AddTxMessage(&hcan1,&TxMessage,TxData,&TxMailbox);
		HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_1); 
		HAL_Delay(100);
		
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void CAN_filterConfig(void){
	CAN_FilterTypeDef filterConfig;
	
	filterConfig.FilterBank=0;
	filterConfig.FilterActivation=ENABLE;
	filterConfig.FilterFIFOAssignment=CAN_RX_FIFO0;
	filterConfig.FilterIdHigh=0x0000;
	filterConfig.FilterIdLow=0x0000;
	filterConfig.FilterMaskIdHigh=0x0000;
	filterConfig.FilterMaskIdLow=0x0000;
	filterConfig.FilterMode=CAN_FILTERMODE_IDMASK;
	filterConfig.FilterScale= CAN_FILTERSCALE_32BIT;
	filterConfig.SlaveStartFilterBank=14;
	
	if(HAL_CAN_ConfigFilter(&hcan1,&filterConfig)!=HAL_OK){
		Error_Handler();
	}
	
	if(HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING)!=HAL_OK){
		Error_Handler();
	}	else{
		flag =1;
	}
		//enable interrupt
	
}
void CAN_Txsetup(){
		if(HAL_CAN_Start(&hcan1)!=HAL_OK){
		Error_Handler();
			
		}
		
		TxMessage.StdId=0x01;
		TxMessage.ExtId=chargerID;
		TxMessage.RTR=CAN_RTR_DATA;
		TxMessage.IDE=CAN_ID_EXT;
		TxMessage.DLC=8;
		TxMessage.TransmitGlobalTime=DISABLE;   //time trigger must be turned ON6
		   
		TxData[0]=voltageControl/256;
		TxData[1]=voltageControl%256;
		TxData[2]=currentControl/256;
		TxData[3]=currentControl%256;
		TxData[4]=1;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
	if(HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&RxMessage,RxData)!=HAL_OK){
		Error_Handler();
	}
	receivedID=RxMessage.ExtId;
	if(receivedID == chargerSentID){
		voltageOutput = (RxData[0]<<8 | RxData[1]);
		currentOutput = (RxData[2]<<8 | RxData[3]);
		status = RxData[4];
		
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
	error = 1;
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
