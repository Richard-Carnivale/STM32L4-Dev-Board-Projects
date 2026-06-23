//
// Created by rich on 5/27/26.
//

#include "user_threads.h"

#include <cmsis_os.h>
#include <stm32l4xx_hal.h>
#include <stdio.h>

#include "main.h"

extern TIM_HandleTypeDef htim1;

extern "C" {
void StartIdleTask_user(void const *argument) {
    for (;;) {
    }
}

void StartTimerTask_user(void const *argument) {
    typedef enum { STATE_STOPPED, STATE_RUNNING } TimerState;
    TimerState currentState = STATE_STOPPED;
    uint8_t lastButtonState = GPIO_PIN_RESET;

    for (;;) {
        uint8_t currentButtonState = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);

        // button press
        if (currentButtonState == GPIO_PIN_SET && lastButtonState == GPIO_PIN_RESET) {
            if (currentState == STATE_STOPPED) {
                // start timer
                HAL_TIM_Base_Start_IT(&htim1);
                currentState = STATE_RUNNING;
            } else {
                // stop timer
                HAL_TIM_Base_Stop_IT(&htim1);

                // reset timer and start it again
                __HAL_TIM_SET_COUNTER(&htim1, 0);
                HAL_TIM_Base_Start_IT(&htim1);
                currentState = STATE_RUNNING;
            }
        }

        // show timer
        if (currentState == STATE_RUNNING) {
            uint32_t timerValue = __HAL_TIM_GET_COUNTER(&htim1);
            printf("Timer: %lu\r\n", timerValue);
        }

        lastButtonState = currentButtonState;
        osDelay(50);
    }
}

void StartToggleLedTask_user(void const *argument) {
    for (;;) {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        osDelay(500);
    }
}
}
