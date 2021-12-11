/******************************************************************************** 
  * @file    GPIO/IOToggle/main.c 
  * @author  MCD Application Team 
  * @version V3.5.0 
  * @date    08-April-2011 
  * @brief   Main program body. 
  ****************************************************************************** 
  * @attention 
  * 
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS 
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE 
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY 
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING 
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE 
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS. 
  * 
  * <h2><center>© COPYRIGHT 2011 STMicroelectronics</center></h2> 
  ****************************************************************************** 
  */ 

/* Includes ------------------------------------------------------------------*/ 
#include <stm32f10x.h> 
#include <stm32f10x_rcc.h> 
#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_rcc.h>
//#include <misc.h> 

/* Private variables ---------------------------------------------------------*/ 

TIM_HandleTypeDef        TimHandle;
void TIM2_IRQHandler(void);

GPIO_InitTypeDef GPIO_InitStructure;
static int greenledval = 0;

void configure_leds(void)
{	 	            
  /* GPIOC Periph clock enable */ 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 

  /* Configure PC6 in output pushpull mode */ 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);	
} 

void configure_timer(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  //TIM_TimeBaseInitTypeDef timerInitStructure; replaced with 
  TIM_Base_InitTypeDef timerInitStructure;
  /*timerInitStructure.TIM_Prescaler = 36000;
  timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  timerInitStructure.TIM_Period = 100;
  timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  timerInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &timerInitStructure);
  TIM_Cmd(TIM2, ENABLE);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); */
  
  timerInitStructure.Prescaler = 36000;
  timerInitStructure.CounterMode = TIM_COUNTERMODE_UP ;
  timerInitStructure.Period = 100;
  timerInitStructure.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  timerInitStructure.RepetitionCounter = 0;
  //TIM_TimeBaseInit(TIM2, &timerInitStructure);
  HAL_TIM_Base_Init(TIM2, &timerInitStructure);
  //TIM_Cmd(TIM2, ENABLE);
  HAL_TIM_Base_Start(&timerInitStructure);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  HAL_TIM_Base_Start_IT
}

HAL_StatusTypeDef  configure_timer2_interrupt()
{
    //NVIC_InitTypeDef nvicStructure;

    /* Configure the TIM2 IRQ priority */
    HAL_NVIC_SetPriority(TIM2_IRQn, TickPriority, 0U);
    /* Enable the TIM2 global Interrupt */
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
    /* Enable TIM2 clock */
    __HAL_RCC_TIM2_CLK_ENABLE();

     /* Initialize TIM2 */
    TimHandle.Instance = TIM2;
  
    /* Initialize TIMx peripheral as follow:
    + Period = [(TIM6CLK/1000) - 1]. to have a (1/1000) s time base.
    + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
    + ClockDivision = 0
    + Counter direction = Up
    */
  TimHandle.Init.Period = (1000000U / 1000U) - 1U;
  TimHandle.Init.Prescaler = uwPrescalerValue;
  TimHandle.Init.ClockDivision = 0;
  TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if(HAL_TIM_Base_Init(&TimHandle) == HAL_OK)
  {
    /* Start the TIM time Base generation in interrupt mode */
    return HAL_TIM_Base_Start_IT(&TimHandle);
  }
  /* Return function status */
  return HAL_ERROR;

    //nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
    //nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
    //nvicStructure.NVIC_IRQChannelSubPriority = 1;
    //nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    //NVIC_Init(&nvicStructure);
}

void TIM2_IRQHandler()
{
   // if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    //{
      //  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
      //  GPIO_WriteBit(GPIOC , GPIO_Pin_6 , (greenledval) ? Bit_SET : Bit_RESET);
	//GPIO_WriteBit(GPIOC , GPIO_Pin_7 , (greenledval) ? Bit_SET : Bit_RESET);
	//greenledval = 1 - greenledval;
    //}

    HAL_TIM_IRQHandler(&TimHandle)
}

int main(void) 
{   
  configure_leds();
  configure_timer();
  configure_timer2_interrupt();

  while (1) 
  { 
  } 
} 

#ifdef  USE_FULL_ASSERT 

/** 
  * @brief  Reports the name of the source file and the source line number 
  *         where the assert_param error has occurred. 
  * @param  file: pointer to the source file name 
  * @param  line: assert_param error line source number 
  * @retval None 
  */ 
void assert_failed(uint8_t* file, uint32_t line) 
{ 
  /* User can add his own implementation to report the file name and line number, 
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */ 

  /* Infinite loop */ 
  wh
