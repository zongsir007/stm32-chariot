#include "servo_control.h"
#include "config.h"

// 引用在 main.c 或 tim.c 中定义的定时器句柄
extern TIM_HandleTypeDef SERVO_TIM_HANDLE;

/**
 * @brief 将角度转换为 PWM 脉宽并写入寄存器
 * @param channel: 定时器通道 (如 TIM_CHANNEL_1)
 * @param angle:   目标角度 (0.0 到 180.0)
 */
void Servo_SetAngle(uint32_t channel, float angle) {
    // 限制角度范围，防止超出机械限位
    if (angle < 0.0f) angle = 0.0f;
    if (angle > 180.0f) angle = 180.0f;

    /* * 舵机标准信号周期为 20ms (50Hz)
     * 0度   -> 0.5ms 脉宽
     * 180度 -> 2.5ms 脉宽
     * 这里的 pulse 计算取决于你的定时器 PSC 和 ARR 配置。
     * 假设你的计数器频率配置为 1MHz (1us 计数一次):
     */
    uint32_t pulse = (uint32_t)(500 + (angle * 2000.0f / 180.0f));

    // 更新 PWM 占空比寄存器 (CCR)
    __HAL_TIM_SET_COMPARE(&SERVO_TIM_HANDLE, channel, pulse);
}
