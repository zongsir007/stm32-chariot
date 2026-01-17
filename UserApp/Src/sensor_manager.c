#include "sensor_manager.h"
#include "config.h"
#include "tim.h"

// 使用定时器 5 进行微秒级计时 (假设时钟频率已配置为 1MHz)
extern TIM_HandleTypeDef htim5;

/**
 * @brief 微秒延时
 */
void delay_us(uint16_t us) {
    uint32_t start = __HAL_TIM_GET_COUNTER(&htim5);
    while ((__HAL_TIM_GET_COUNTER(&htim5) - start) < us);
}

/**
 * @brief  获取超声波距离 (带防卡死保护)
 */
float Ultrasonic_GetDistance(GPIO_TypeDef* TrigPort, uint16_t TrigPin, GPIO_TypeDef* EchoPort, uint16_t EchoPin) {
    uint32_t start_time = 0;

    // 启动定时器
    HAL_TIM_Base_Start(&htim5);

    // 发送触发信号
    HAL_GPIO_WritePin(TrigPort, TrigPin, GPIO_PIN_RESET);
    delay_us(2);
    HAL_GPIO_WritePin(TrigPort, TrigPin, GPIO_PIN_SET);
    delay_us(12);
    HAL_GPIO_WritePin(TrigPort, TrigPin, GPIO_PIN_RESET);

    // 1. 等待 Echo 变高 (增加 2ms 超时)
    uint32_t wait_high = 0;
    while (HAL_GPIO_ReadPin(EchoPort, EchoPin) == GPIO_PIN_RESET) {
        if (++wait_high > 2000) return 999.0f; // 没连接传感器时返回大数值，不触发避障
        delay_us(1);
    }

    start_time = __HAL_TIM_GET_COUNTER(&htim5);

    // 2. 等待 Echo 变低 (量程限幅 20ms, 约 3.4 米)
    while (HAL_GPIO_ReadPin(EchoPort, EchoPin) == GPIO_PIN_SET) {
        if (__HAL_TIM_GET_COUNTER(&htim5) - start_time > 20000) break;
    }

    uint32_t duration = __HAL_TIM_GET_COUNTER(&htim5) - start_time;

    // 距离计算 cm
    return (float)duration / 58.0f;
}
