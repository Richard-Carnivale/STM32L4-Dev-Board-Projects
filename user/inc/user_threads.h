//
// Created by rich on 5/27/26.
//

#ifndef DEVBOARD_STM32L4_USER_THREADS_H
#define DEVBOARD_STM32L4_USER_THREADS_H

#ifdef __cplusplus
extern "C" {
#endif

    void StartIdleTask_user(void const * argument);
    void StartTimerTask_user(void const * argument);
    void StartToggleLedTask_user(void const *argument);

#ifdef __cplusplus
}
#endif

#endif // DEVBOARD_STM32L4_USER_THREADS_H
