#ifndef __MOTOR_DRIVER_H
#define __MOTOR_DRIVER_H

#include "main.h"
#include "can.h"

// PID 控制器结构体
typedef struct {
    float kp, ki, kd;
    float target;        // 目标值
    float current;       // 当前值
    float error;         // 偏差
    float last_error;    // 上次偏差
    float integral;      // 积分累加
    float output;        // 计算输出
    float max_output;    // 输出限幅
    float max_integral;  // 积分限幅
} PID_TypeDef;

// 电机状态结构体
typedef struct {
    int16_t rpm;          // 实际转速 (来自CAN反馈)
    int16_t target_rpm;   // 目标转速 (来自控制算法)
    int16_t torque_current;
    uint16_t angle;
} Motor_Status_t;

extern Motor_Status_t Motor_Stats[4];
extern PID_TypeDef Motor_PID[4];

// 函数声明
void PID_Init(void);
float PID_Compute(PID_TypeDef *pid, float target, float current);
void Motor_Send_Control_Group(int16_t v1, int16_t v2, int16_t v3, int16_t v4);
void Motor_Stop_All(void);
void CAN_Filter_Config(void);

#endif
