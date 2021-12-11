/**
  ******************************************************************************
  * @file    BSP/Src/main.c 
  * @author  MCD Application Team
  * @brief   Main program body
  ******************************************************************************

  * @brief  Main program
  * @param  None
  * @retval None
  */

#include <stdio.h>
#include <unistd.h>

int main(void)
{
  int i;
  char hi[13]="Hello World!";
  
  for ( i=0; i != 13; i++)
    {
      printf("Hello WOrld!\n");
      sleep(1);
      }

}
