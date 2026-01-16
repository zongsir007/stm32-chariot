#ifndef __MOTION_CONTROL_H
#define __MOTION_CONTROL_H

#include "main.h"

/**
 * @brief 常用运动指令封装
 */
void Move_Forward(int16_t speed);
void Move_Backward(int16_t speed);
void Turn_Left(int16_t linear_speed, int16_t rotate_speed);
void Turn_Right(int16_t linear_speed, int16_t rotate_speed);
void Rotate_Left(int16_t rotate_speed);
void Rotate_Right(int16_t rotate_speed);
void Chassis_Stop(void);

/**
 * @brief 核心控制逻辑：线速度与角速度混合计算
 * @param v_linear  前进/后退速度 (正数前进)
 * @param v_angular 转向速度 (正数左转，负数右转)
 */
void Chassis_Drive(int16_t v_linear, int16_t v_angular);

#endif
