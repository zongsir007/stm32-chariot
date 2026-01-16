#ifndef __CONFIG_H
#define __CONFIG_H
#include "tim.h"    // 只有你在 .ioc 里配置了定时器后，这个才有效
#include "main.h"

/* ==========================================================================
 * 1. CAN 总线与电机 ID 配置 (APB1: 42MHz)
 * ========================================================================== */
#define CAN_BUS_HANDLE         hcan1

#define MOTOR_FL_ID            0x01  // 前左电机 (Front Left)
#define MOTOR_FR_ID            0x02  // 前右电机 (Front Right)
#define MOTOR_RL_ID            0x03  // 后左电机 (Rear Left)
#define MOTOR_RR_ID            0x04  // 后右电机 (Rear Right)

#define MOTOR_FEEDBACK_BASE_ID 0x200 // 假设电机回传偏移量，ID未定

/* ==========================================================================
 * 2. 舵机 PWM 配置 (APB1: 84MHz, TIM2)
 * ========================================================================== */
#define SERVO_TIM_HANDLE       htim2

// 对应 TIM2 的四个通道
#define SERVO_CH_1             TIM_CHANNEL_1
#define SERVO_CH_2             TIM_CHANNEL_2
#define SERVO_CH_3             TIM_CHANNEL_3
#define SERVO_CH_4             TIM_CHANNEL_4

// 舵机脉宽定义 (单位: us, 对应 ARR=20000)
#define SERVO_MIN_PULSE        500   // 0度
#define SERVO_MID_PULSE        1500  // 90度
#define SERVO_MAX_PULSE        2500  // 180度

/* ==========================================================================
 * 3. 传感器引脚配置 (根据你的实际接线修改 Pin 和 Port)
 * ========================================================================== */
// 超声波 1 (例如：正前方)
#define US1_TRIG_PORT          GPIOB		//触发信号输出端口
#define US1_TRIG_PIN           GPIO_PIN_0	//触发信号引脚
#define US1_ECHO_PORT          GPIOB		//回响信号输入端口
#define US1_ECHO_PIN           GPIO_PIN_1	//回响信号输出端口

// 超声波 2 (例如：正后方)
#define US2_TRIG_PORT          GPIOB
#define US2_TRIG_PIN           GPIO_PIN_2
#define US2_ECHO_PORT          GPIOB
#define US2_ECHO_PIN           GPIO_PIN_3

// 巡线传感器
#define LINE_L_PORT            GPIOE		//左侧巡线端口
#define LINE_L_PIN             GPIO_PIN_0	//左侧巡线引脚
#define LINE_R_PORT            GPIOE
#define LINE_R_PIN             GPIO_PIN_1

/* ==========================================================================
 * 4. 运动控制参数
 * ========================================================================== */
#define SAFE_DISTANCE          25.0f // 避障触发距离 (cm)
#define WHEEL_SPEED_MAX        4000  // 电机最大转速限制
#define PID_PERIOD_MS          10    // PID控制周期（10ms=100Hz）

#endif /* __CONFIG_H */
