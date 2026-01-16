#include "motion_control.h"
#include "motor_driver.h"
#include "config.h"

/**
 * @brief 差速核心驱动函数
 */
void Chassis_Drive(int16_t v_linear, int16_t v_angular) {
    // 1. 差速叠加计算
    // 左边轮子 = 前进速度 - 转向速度 (左转时左轮减速)
    // 右边轮子 = 前进速度 + 转向速度 (左转时右轮加速)
    int16_t left_v  = v_linear - v_angular;
    int16_t right_v = v_linear + v_angular;

    // 2. 限速处理：确保不会超过 config.h 里的最大值
    if (left_v  > WHEEL_SPEED_MAX)  left_v  = WHEEL_SPEED_MAX;
    if (left_v  < -WHEEL_SPEED_MAX) left_v  = -WHEEL_SPEED_MAX;
    if (right_v > WHEEL_SPEED_MAX)  right_v = WHEEL_SPEED_MAX;
    if (right_v < -WHEEL_SPEED_MAX) right_v = -WHEEL_SPEED_MAX;

    // 3. 映射到物理电机
    // 【重要提示】：由于右侧电机（FR, RR）镜像安装，方向是反的
    // 所以给右侧电机的参数前要加 负号 (-)
    // 顺序对应：FL, FR, RL, RR
    Motor_Send_Control_Group(left_v, -right_v, left_v, -right_v);
}

void Move_Forward(int16_t speed) {
    Chassis_Drive(speed, 0);
}

void Move_Backward(int16_t speed) {
    Chassis_Drive(-speed, 0);
}

void Rotate_Left(int16_t rotate_speed) {
    // 原地左转：前进速度为0，转向速度为正
    Chassis_Drive(0, rotate_speed);
}

void Rotate_Right(int16_t rotate_speed) {
    // 原地右转：前进速度为0，转向速度为负
    Chassis_Drive(0, -rotate_speed);
}

void Chassis_Stop(void) {
    Motor_Stop_All(); // 直接调用 motor_driver 里的全停函数
}
