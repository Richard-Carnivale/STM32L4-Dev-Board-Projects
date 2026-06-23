//
// Created by rich on 5/27/26.
//

#include "user_threads.h"

#include <cmsis_os.h>
#include <stm32l4xx_hal.h>

#include "main.h"

extern TIM_HandleTypeDef htim1;

extern "C" {
    void StartDefaultTask_user(void const * argument)
    {
        HAL_TIM_Base_Start_IT(&htim1);
        
        for (;;)
        {
            HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
            if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)) {

            }
            osDelay(200);
        }
    }
}