#ifndef __MOTOR_DRIVER_H
#define __MOTOR_DRIVER_H

#include "main.h"
#include "can.h"

/**
 * @brief 电机状态结构体
 */
typedef struct {
    int16_t rpm;            // 实际转速
    int16_t target_rpm;     // 目标转速
    int16_t torque_current; // 电流反馈 (步进电机可能映射为负载率)
    uint16_t angle;         // 角度反馈
} Motor_Status_t;

// 外部声明
extern Motor_Status_t Motor_Stats[4];

/**
 * @brief 核心驱动函数声明
 */
void CAN_Filter_Config(void);
void Motor_Send_Control_Group(int16_t v1, int16_t v2, int16_t v3, int16_t v4);
void Motor_Send_Control_Single(uint16_t std_id, int16_t rpm); // 建议也声明单机控制
void Motor_Stop_All(void);
void Motor_Enable_All(void);

#endif /* __MOTOR_DRIVER_H */
