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
  * COPYRIGHT(c) 2018 STMicroelectronics
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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "my_files/sound_samples.h"
#include "my_files/accelerometer.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
volatile uint8_t    current_sound_pattern;
volatile uint32_t   total_samples_count;
volatile uint32_t   current_samples_count;
volatile uint8_t    button_pushed_flag;
volatile uint8_t    random_byte;
uint8_t             hit_pattern;
uint8_t             if_saber_turned_on_flag;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

void print_string(char *data)
{
  char buf[256] = { 0 };
  sprintf(&buf[0], "%s%s\r\n", "\033[3J", data); // first string 'clears the scrollbar' in putty
  HAL_UART_Transmit(&huart2, (uint8_t *)&buf[0], sizeof(buf), 100);
}

void set_current_sound_pattern(uint8_t val)
{
  HAL_TIM_Base_Stop_IT(&htim10);
  current_sound_pattern = val;
  total_samples_count = sound_structs_array[val]->array_size;
  current_samples_count = 0;
  HAL_TIM_Base_Start_IT(&htim10);
}

uint8_t get_hit_pattern(void)
{
  // hit_pattern++;
  // if(hit_pattern < HIT1_PATTERN || hit_pattern > LAST_HIT_NUMBER) 
  // {
  //   hit_pattern = HIT1_PATTERN;
  // }
  // return hit_pattern;
  return (random_byte % (LAST_HIT_NUMBER - HIT1_PATTERN + 1)) + HIT1_PATTERN;    // random hit pattern
}

void power_up_saber(void)
{
  HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_8B_R, sound_structs_array[POWER_UP_PATTERN]->array_ptr[current_samples_count]);
  // random_byte = current_samples_count & 0xFF;        random byte not needed while powering up/down for now
  TIM3->CCR1 = (TIM3->CCR1 >= 255) ? 255 : (current_samples_count >> 6) & 0xFF;
  if(current_samples_count > (total_samples_count >> 1)) 
  {
    TIM3->CCR1 = 255;
    if_saber_turned_on_flag = 1;
  }
  if(++current_samples_count >= total_samples_count)
  {
    set_current_sound_pattern(IDLE_PATTERN);
    // if_saber_turned_on_flag = 1;
  }
}

void power_down_saber(void)
{
  if_saber_turned_on_flag = 0;
  HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_8B_R, sound_structs_array[POWER_DOWN_PATTERN]->array_ptr[current_samples_count]);
  // random_byte = current_samples_count & 0xFF;
  TIM3->CCR1 = (TIM3->CCR1 == 0) ? 0 : 255 - ((current_samples_count >> 6) & 0xFF);
  if(++current_samples_count >= total_samples_count)
  {
    HAL_TIM_Base_Stop_IT(&htim10);
    TIM3->CCR1 = 0;
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM10)
  {
    if(current_sound_pattern == POWER_UP_PATTERN) power_up_saber();
    else if(current_sound_pattern == POWER_DOWN_PATTERN) power_down_saber();
    else 
    {
        HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_8B_R, sound_structs_array[current_sound_pattern]->array_ptr[current_samples_count]);
        random_byte = current_samples_count & 0xFF;
        if(++current_samples_count >= total_samples_count) set_current_sound_pattern(IDLE_PATTERN);
    }
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == GPIO_PIN_13 || GPIO_Pin == SIGNALQUEST_Pin) button_pushed_flag = 1;
}

void button_pushed(void)
{
  if(button_pushed_flag)
    {
      HAL_Delay(15);                // debouncing the button
      uint16_t button_counter = 0;
      while(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13))
      {
        HAL_Delay(1);
        button_counter++;
        if(button_counter > 750)
        {
          set_current_sound_pattern((if_saber_turned_on_flag) ? POWER_DOWN_PATTERN : POWER_UP_PATTERN);
          break;
        }
      }
      if(button_counter <= 750 && if_saber_turned_on_flag && (current_sound_pattern == IDLE_PATTERN || current_sound_pattern == POWER_UP_PATTERN
                               || (current_sound_pattern >= HIT1_PATTERN && current_sound_pattern <= LAST_HIT_NUMBER 
                               && current_samples_count > (total_samples_count >> 2)))) set_current_sound_pattern(get_hit_pattern());
      button_pushed_flag = 0;
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
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();

  /* USER CODE BEGIN 2 */
  HAL_DAC_Start(&hdac, DAC1_CHANNEL_1);
  HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_8B_R, 0);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  TIM3->CCR1 = 0;                             // TIM capture/compare register for PWM generating
  hit_pattern = 0;
  button_pushed_flag = 0;
  if_saber_turned_on_flag = 0;
  random_byte = 0;
  print_string("Lightsaber started..");

  uint8_t reg_settings = 0b00000011;    // setting CTRL_REG1 to 680ms !160ms meas delay, Fast Read Mode and Active Mode
  HAL_I2C_Mem_Write(&hi2c1, MMA845_ADDR, 0x2a, 1, &reg_settings, 1, 100);
  
  // reg_settings = 0b00011100;
  // HAL_I2C_Mem_Write(&hi2c1, MMA845_ADDR, 0x2b, 1, &reg_settings, 1, 100);

  char text[255] = { 0 };
  uint8_t data_buf[3] = { 0 };
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  { 
    while(!(data_buf[0] & (1<<3)))  // checking ZYXDR bit inF_MODE register
    {
      HAL_I2C_Mem_Read(&hi2c1, MMA845_ADDR, 0x00, 1, &data_buf[0], 1, 100);
    }
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    HAL_I2C_Mem_Read(&hi2c1, MMA845_ADDR, 0x01, 1, &data_buf[0], 1, 100); // X
    HAL_I2C_Mem_Read(&hi2c1, MMA845_ADDR, 0x03, 1, &data_buf[1], 1, 100); // Y
    HAL_I2C_Mem_Read(&hi2c1, MMA845_ADDR, 0x05, 1, &data_buf[2], 1, 100); // Z

    sprintf(&text[0], "X: %d\r\nY: %d\r\nZ: %d\r\n", (int8_t)data_buf[0], (int8_t)data_buf[1], (int8_t)data_buf[2]);
    HAL_UART_Transmit(&huart2, (uint8_t *)&text[0], sizeof(text), 100);
    data_buf[0] = 0;
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
