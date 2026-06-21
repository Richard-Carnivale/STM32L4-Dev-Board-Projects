//
// Created by rich on 5/27/26.
//

#include "user_threads.h"

#include <cmsis_os.h>
#include <stm32l4xx_hal.h>

#include "main.h"

extern "C" {
    void StartDefaultTask_user(void const * argument)
    {
        for (;;)
        {
            HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
            osDelay(200);
        }
    }
}