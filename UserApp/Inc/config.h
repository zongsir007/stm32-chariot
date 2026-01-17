#ifndef __CONFIG_H
#define __CONFIG_H

#include "tim.h"
#include "main.h"

/* ==========================================================================
 * 1. CAN 总线与电机 ID 配置 (APB1: 42MHz)
 * ========================================================================== */
// 核心句柄
#define CAN_BUS_HANDLE         hcan1

// 电机物理 ID (必须与电机背后拨码开关一致)
#define MOTOR_FL_ID            0x01  // 前左 (Front Left)
#define MOTOR_FR_ID            0x02  // 前右 (Front Right)
#define MOTOR_RL_ID            0x03  // 后左 (Rear Left)
#define MOTOR_RR_ID            0x04  // 后右 (Rear Right)

// 张大头电机反馈 ID 通常与发送 ID 相同，此处保留用于逻辑判断
#define MOTOR_FEEDBACK_BASE_ID 0x01

/* ==========================================================================
 * 2. 舵机 PWM 配置 (保持默认，暂不调试)
 * ========================================================================== */
#define SERVO_TIM_HANDLE       htim2
#define SERVO_CH_1             TIM_CHANNEL_1
#define SERVO_CH_2             TIM_CHANNEL_2
#define SERVO_CH_3             TIM_CHANNEL_3
#define SERVO_CH_4             TIM_CHANNEL_4

#define SERVO_MIN_PULSE        500   // 0.5ms (0度)
#define SERVO_MID_PULSE        1500  // 1.5ms (90度)
#define SERVO_MAX_PULSE        2500  // 2.5ms (180度)

/* ==========================================================================
 * 3. 传感器引脚配置 (超声波)
 * ========================================================================== */
#define US1_TRIG_PORT          GPIOB
#define US1_TRIG_PIN           GPIO_PIN_0
#define US1_ECHO_PORT          GPIOB
#define US1_ECHO_PIN           GPIO_PIN_1

#define US2_TRIG_PORT          GPIOB
#define US2_TRIG_PIN           GPIO_PIN_2
#define US2_ECHO_PORT          GPIOB
#define US2_ECHO_PIN           GPIO_PIN_3

/* ==========================================================================
 * 4. 运动控制参数 (针对张大头步进电机优化)
 * ========================================================================== */
#define SAFE_DISTANCE          30.0f  // 厘米 (cm)//超声波安全距离

#define WHEEL_SPEED_MAX        300    //每分钟最大转速

// 逻辑循环周期
#define PID_PERIOD_MS          10

#endif /* __CONFIG_H */
