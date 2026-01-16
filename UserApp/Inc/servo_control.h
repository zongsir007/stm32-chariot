#ifndef __SERVO_CONTROL_H
#define __SERVO_CONTROL_H

#include "main.h"

/**
 * @brief 设置舵机角度
 * @param channel 定时器通道 (例如 SERVO_CH_1 即 TIM_CHANNEL_1)
 * @param angle   目标角度 (0.0f 到 180.0f)
 */
void Servo_SetAngle(uint32_t channel, float angle);

#endif /* __SERVO_CONTROL_H */
