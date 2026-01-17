#include "motion_control.h"
#include "motor_driver.h"
#include "config.h"

/**
 * @brief 差速核心驱动函数 (适配步进电机速度模式)
 */
void Chassis_Drive(int16_t v_linear, int16_t v_angular) {
    // 1. 差速叠加计算
    // 左边轮子 = 前进速度 - 转向速度 (左转时，左轮减速/倒转)
    // 右边轮子 = 前进速度 + 转向速度 (左转时，右轮加速/正转)
    int16_t left_v  = v_linear - v_angular;
    int16_t right_v = v_linear + v_angular;

    // 2. 限速处理
    // 注意：步进电机的 WHEEL_SPEED_MAX 建议在 config.h 中设为 300-600 RPM
    if (left_v  > WHEEL_SPEED_MAX)  left_v  = WHEEL_SPEED_MAX;
    if (left_v  < -WHEEL_SPEED_MAX) left_v  = -WHEEL_SPEED_MAX;
    if (right_v > WHEEL_SPEED_MAX)  right_v = WHEEL_SPEED_MAX;
    if (right_v < -WHEEL_SPEED_MAX) right_v = -WHEEL_SPEED_MAX;

    // 3. 映射到物理电机
    // 提示：张大头电机的 Motor_Send_Control_Single 内部已处理正负号(tx_data[1])
    // 如果发现右侧轮子转反了，再修改此处为 -right_v
    // 对应 ID 顺序: 1(FL), 2(FR), 3(RL), 4(RR)
    Motor_Send_Control_Group(left_v, right_v, left_v, right_v);
}

/**
 * @brief 直行控制
 */
void Move_Forward(int16_t speed) {
    Chassis_Drive(speed, 0);
}

void Move_Backward(int16_t speed) {
    Chassis_Drive(-speed, 0);
}

/**
 * @brief 原地转向控制
 */
void Rotate_Left(int16_t rotate_speed) {
    // 原地左转：前进分量为0，转向分量为正
    Chassis_Drive(0, rotate_speed);
}

void Rotate_Right(int16_t rotate_speed) {
    // 原地右转：前进分量为0，转向分量为负
    Chassis_Drive(0, -rotate_speed);
}

/**
 * @brief 混合转向控制 (边走边转)
 */
void Turn_Left(int16_t linear_speed, int16_t rotate_speed) {
    Chassis_Drive(linear_speed, rotate_speed);
}

void Turn_Right(int16_t linear_speed, int16_t rotate_speed) {
    Chassis_Drive(linear_speed, -rotate_speed);
}

/**
 * @brief 立即停止所有电机
 */
void Chassis_Stop(void) {
    Motor_Stop_All();
}
