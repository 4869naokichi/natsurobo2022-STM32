/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include "stdio.h"
#include "MPU9250.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint8_t DS3_BTNS_DATMAP_BYTE[16] = {
    2, 2, 2, 2, 2, 2,/* 0, 0,*/ 2, 2, 2, 1, 1, 1, 1, 1
};

uint8_t DS3_BTNS_DATMAP_BITMASK[16] = {
    0x03, 0x0C, 0x03, 0x0C, 0x03, 0x0C,/* 0x00, 0x00,*/
    0x10, 0x40, 0x20, 0x01, 0x04, 0x10, 0x02, 0x08,
};

uint8_t DS3_BTNS_DATMAP_FLAGBIT[16] = {
    0x01, 0x04, 0x02, 0x08, 0x03, 0x0C,/* 0x00, 0x00,*/
    0x10, 0x40, 0x20, 0x01, 0x04, 0x10, 0x02, 0x08,
};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t SBDBTRxData[8];
int buttonsIsPressed[16] = {0};
float LX = 0.0f;
float LY = 0.0f;
float RX = 0.0f;
float RY = 0.0f;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Analyze_Raw_Data();

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
	setbuf(stdout, NULL);

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
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_SPI3_Init();
  MX_TIM5_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  LL_TIM_EnableCounter(TIM2);
  LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
  LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH2);
  LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH3);
  LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH4);
  LL_TIM_OC_SetCompareCH1(TIM2, 1000);
  LL_TIM_OC_SetCompareCH2(TIM2, 1000);
  LL_TIM_OC_SetCompareCH3(TIM2, 1000);
  LL_TIM_OC_SetCompareCH4(TIM2, 1000);

  LL_TIM_EnableCounter(TIM3);
  LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);
  LL_TIM_OC_SetCompareCH1(TIM3, 145);
  LL_TIM_EnableCounter(TIM4);
  LL_TIM_CC_EnableChannel(TIM4, LL_TIM_CHANNEL_CH1);
  LL_TIM_OC_SetCompareCH1(TIM4, 145);

  LL_TIM_EnableCounter(TIM5);
  LL_TIM_CC_EnableChannel(TIM5, LL_TIM_CHANNEL_CH1);
  LL_TIM_CC_EnableChannel(TIM5, LL_TIM_CHANNEL_CH2);
  LL_TIM_OC_SetCompareCH1(TIM5, 1000);
  LL_TIM_OC_SetCompareCH2(TIM5, 1000);

  MPU9250_Init();
  printf("Calibration\r\n");
  MPU9250_Calibration();

  HAL_UART_Receive_IT(&huart1, SBDBTRxData, 8);

  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  static int count;

	  static float theta;
	  static float ref = 0;
	  theta += MPU9250_Read_GyroZ()*0.013f;

	  float Lr = sqrt(LX*LX+LY*LY);
	  float Rr = sqrt(RX*RX+RY*RY);

	  if (Lr > 64.0f) {
		  LX = LX * (64.0f/Lr);
		  LY = LY * (64.0f/Lr);
		  Lr = sqrt(LX*LX+LY*LY);
	  }
	  if (Rr > 64.0f) {
		  RX = RX * (64.0f/Rr);
		  RY = RY * (64.0f/Rr);
		  Rr = sqrt(RX*RX+RY*RY);
	  }

	  const float a = 0.155f;
	  const float b = 0.155f;
	  float vx = LX*0.0f;
	  //float vx = LY*10.0f;
	  float vy = LY*0.0f;
	  //float vy = LX*(-10.0f);
	  float omega;
	  if (buttonsIsPressed[0]) {
		  vy = 200.0f;
		  //vx = 200.0f;
	  } else if (buttonsIsPressed[1]) {
		  vx = 200.0f;
		  //vy = -200.0f;
	  } else if (buttonsIsPressed[2]) {
		  vy = -200.0f;
		  //vx = -200.0f;
	  } else if (buttonsIsPressed[3]) {
		  vx = -200.0f;
		  //vy = 200.0f;
	  } else {
		  ;
	  }
	  if (buttonsIsPressed[12]) {
		  //omega = 700;
		  ref += 0.5f;
	  } else if (buttonsIsPressed[13]) {
		  //omega = -700;
		  ref -= 0.5f;
	  } else {
		  //omega = 0;
	  }
	  omega = (ref-theta)*200;

	  float v[4];
	  v[0] = -vx + vy + (a+b)*omega;
	  v[1] = +vx + vy - (a+b)*omega;
	  v[2] = -vx + vy - (a+b)*omega;
	  v[3] = +vx + vy + (a+b)*omega;

	  float max = fabs(v[0]);
	  for (int i=0; i<4; i++){
		  if (fabs(v[i]) > max) {
			  max = fabs(v[i]);
		  }
	  }
	  if (max > 1000) {
		  for (int i=0; i<4; i++) {
			  v[i] *= (1000/max);
		  }
	  }

	  /*
	  if (v[0] > 1000) v[0] = 1000;
	  if (v[0] < -1000) v[0] = -1000;
	  if (v[1] > 1000) v[1] = 1000;
	  if (v[1] < -1000) v[1] = -1000;
	  if (v[2] > 1000) v[2] = 1000;
	  if (v[2] < -1000) v[2] = -1000;
	  if (v[3] > 1000) v[3] = 1000;
	  if (v[3] < -1000) v[3] = -1000;
	  */

	  v[0] *= -1;
	  v[2] *= -1;

	  LL_TIM_OC_SetCompareCH1(TIM2, (int)(v[0]+1000));
	  LL_TIM_OC_SetCompareCH2(TIM2, (int)(v[1]+1000));
	  LL_TIM_OC_SetCompareCH3(TIM2, (int)(v[2]+1000));
	  LL_TIM_OC_SetCompareCH4(TIM2, (int)(v[3]+1000));

	  LL_TIM_OC_SetCompareCH1(TIM5, 1300);

	  if (RX > 1.0f) { //取り込みアーム
		  LL_TIM_OC_SetCompareCH1(TIM5, 100);
	  } else if (RX < -1.0f){
		  LL_TIM_OC_SetCompareCH1(TIM5, 1900);
	  } else {
		  LL_TIM_OC_SetCompareCH1(TIM5, 1000);
	  }

	  /*
	  if (RY < -1.0f) { //取り込みアーム
		  LL_TIM_OC_SetCompareCH1(TIM5, 1900);
	  } else if (RY > 1.0f){
		  LL_TIM_OC_SetCompareCH1(TIM5, 100);
	  } else {
		  LL_TIM_OC_SetCompareCH1(TIM5, 1000);
	  }
	  */

	  if (buttonsIsPressed[9]) { //サーボ
		  LL_TIM_OC_SetCompareCH1(TIM3, 70);
		  LL_TIM_OC_SetCompareCH1(TIM4, 80);
	  } else if (buttonsIsPressed[4]){
		  LL_TIM_OC_SetCompareCH1(TIM3, 180);
		  LL_TIM_OC_SetCompareCH1(TIM4, 180);
	  } else {
		  //LL_TIM_OC_SetCompareCH1(TIM3, 145);
	  }

	  if (buttonsIsPressed[7]) { //駒アーム
		  HAL_GPIO_WritePin(AIR_GPIO_Port, AIR_Pin, GPIO_PIN_SET);
	  } else {
		  HAL_GPIO_WritePin(AIR_GPIO_Port, AIR_Pin, GPIO_PIN_RESET);
	  }

	  static int prev6;
	  static int prev8;
	  if (prev6==0 && buttonsIsPressed[6]) { //参の目
		  count = 8;
	  } else if (prev8==0 && buttonsIsPressed[8]) { //壱の目
		  count = 2;
	  } else if (count > 0) {
		  count--;
	  } else {
		  ;
	  }
	  prev6 = buttonsIsPressed[6];
	  prev8 = buttonsIsPressed[8];

	  if (count > 0) {
		  HAL_GPIO_WritePin(CUBE_GPIO_Port, CUBE_Pin, GPIO_PIN_SET);
	  } else {
		  HAL_GPIO_WritePin(CUBE_GPIO_Port, CUBE_Pin, GPIO_PIN_RESET);
	  }

	  if (buttonsIsPressed[5]) { //あがり
		  HAL_GPIO_WritePin(AGARI_GPIO_Port, AGARI_Pin, GPIO_PIN_SET);;
	  } else {
		  HAL_GPIO_WritePin(AGARI_GPIO_Port, AGARI_Pin, GPIO_PIN_RESET);;
	  }

	  /*
	  for(int i=0; i<16; i++){
		  printf("%d ", buttonsIsPressed[i]);
	  }
	  printf("\r\n");
	  */

	  printf("LX=%f LY=%f RX=%f RY=%f %f %f\r\n", LX, LY, RX, RY, Lr, Rr);
	  //printf("v1=%f v2=%f v3=%f v4=%f\r\n", v[0], v[1], v[2], v[3]);
	  //printf("v1=%d v2=%d v3=%d v4=%d\r\n", (int)(v1+1000), (int)(v2+1000), (int)(v3+1000), (int)(v4+1000));
	  //printf("%f\r\n", theta);
	  HAL_Delay(10);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, 10);
	return len;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    HAL_UART_Receive_IT(&huart1, SBDBTRxData, 8);
    Analyze_Raw_Data();
}

void Analyze_Raw_Data()
{
    for(int i=0; i<16; i++){
    	int f = (SBDBTRxData[DS3_BTNS_DATMAP_BYTE[i]] & DS3_BTNS_DATMAP_BITMASK[i]) == DS3_BTNS_DATMAP_FLAGBIT[i];
        buttonsIsPressed[i] = f;
    }
    LX = (float)(SBDBTRxData[3]) - 64.0f;
    LY = 64.0f - (float)(SBDBTRxData[4]);
    RX = (float)(SBDBTRxData[5]) - 64.0f;
    RY = 64.0f - (float)(SBDBTRxData[6]);
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

