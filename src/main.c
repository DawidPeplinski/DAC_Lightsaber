/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l1xx_hal.h"
#include "dac.h"
#include "tim.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "idle_samples.h"
#include "power_up_samples.h"
#include "power_down_samples.h"
#include "hit1_samples.h"
#include "hit2_samples.h"
#include "hit3_samples.h"
#include "hit4_samples.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define POWER_UP_PATTERN      0
#define IDLE_PATTERN          1
#define HIT1_PATTERN          2
#define HIT2_PATTERN          3
#define HIT3_PATTERN          4
#define HIT4_PATTERN          5
#define POWER_DOWN_PATTERN    6

volatile uint8_t    sound_pattern;
volatile uint32_t   samples_count;
volatile uint32_t   current_count;
volatile uint8_t    button_flag;
uint8_t             hit_pattern;
uint8_t             if_turned_on_flag;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void set_sound_pattern(uint8_t val)
{
  HAL_TIM_Base_Stop_IT(&htim10);
  sound_pattern = val;
  switch(val)
  {
    case POWER_UP_PATTERN: samples_count = sizeof(power_up_samples); break;
    case IDLE_PATTERN: samples_count = sizeof(idle_samples); break;
    case HIT1_PATTERN: samples_count = sizeof(hit1_samples); break;
    case HIT2_PATTERN: samples_count = sizeof(hit2_samples); break;
    case HIT3_PATTERN: samples_count = sizeof(hit3_samples); break;
    case HIT4_PATTERN: samples_count = sizeof(hit4_samples); break;
    case POWER_DOWN_PATTERN: samples_count = sizeof(power_down_samples); break;
  }
  current_count = 0;
  HAL_TIM_Base_Start_IT(&htim10);
}

uint8_t get_hit_pattern(void)
{
  hit_pattern++;
  if(hit_pattern < HIT1_PATTERN || hit_pattern > HIT4_PATTERN) 
  {
    hit_pattern = HIT1_PATTERN;
  }
  return hit_pattern;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM10)
  {
      switch(sound_pattern)
      {
        case POWER_UP_PATTERN: 
        {
          HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_8B_R, power_up_samples[current_count]);
          if(++current_count >= samples_count) set_sound_pattern(IDLE_PATTERN);
        } break;

        case IDLE_PATTERN: 
        {
          HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_8B_R, idle_samples[current_count]);
          if(++current_count >= samples_count) current_count = 0;     // looping the idle mode
        } break;

        case HIT1_PATTERN:
        {
          HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_8B_R, hit1_samples[current_count]);
          if(++current_count >= samples_count) set_sound_pattern(IDLE_PATTERN);
        } break;
        case HIT2_PATTERN:
        {
          HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_8B_R, hit2_samples[current_count]);
          if(++current_count >= samples_count) set_sound_pattern(IDLE_PATTERN);
        } break;
        case HIT3_PATTERN:
        {
          HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_8B_R, hit3_samples[current_count]);
          if(++current_count >= samples_count) set_sound_pattern(IDLE_PATTERN);
        } break;
        case HIT4_PATTERN:
        {
          HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_8B_R, hit4_samples[current_count]);
          if(++current_count >= samples_count) set_sound_pattern(IDLE_PATTERN);
        } break;
        case POWER_DOWN_PATTERN:
        {
          HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_8B_R, power_down_samples[current_count]);
          if(++current_count >= samples_count) HAL_TIM_Base_Stop_IT(&htim10);
        } break;
      }
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == GPIO_PIN_13)
  {
    button_flag = 1;
  }
}

void button_pushed(void)
{
  if(button_flag)
    {
      HAL_Delay(15);                // debouncing the button
      uint16_t button_counter = 0;
      while(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13))
      {
        HAL_Delay(1);
        button_counter++;
        if(button_counter > 750)
        {
          set_sound_pattern((if_turned_on_flag) ? POWER_DOWN_PATTERN : POWER_UP_PATTERN);
          if_turned_on_flag = !if_turned_on_flag;
          break;
        }
      }
      if(button_counter <= 750 && if_turned_on_flag) set_sound_pattern(get_hit_pattern());
      button_flag = 0;
    }
}

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_DAC_Init();
  MX_TIM10_Init();

  /* USER CODE BEGIN 2 */
  HAL_DAC_Start(&hdac, DAC1_CHANNEL_1);
  HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_8B_R, 0);
  // set_sound_pattern(POWER_UP_PATTERN);
  hit_pattern = 0;
  button_flag = 0;
  if_turned_on_flag = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  { 
    button_pushed();
  }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
