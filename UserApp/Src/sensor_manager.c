#include "sensor_manager.h"
#include "config.h"
#include "tim.h"

extern TIM_HandleTypeDef htim5;

/**
 * @brief  微秒级精准延时
 */
void delay_us(uint16_t us) {
    uint32_t start = __HAL_TIM_GET_COUNTER(&htim5);
    while ((__HAL_TIM_GET_COUNTER(&htim5) - start) < us);
}

/**
 * @brief  获取超声波传感器距离 (cm)
 */
float Ultrasonic_GetDistance(GPIO_TypeDef* TrigPort, uint16_t TrigPin, GPIO_TypeDef* EchoPort, uint16_t EchoPin) {
    uint32_t start_time = 0;
    uint32_t end_time = 0;

    // 1. 确保定时器已启动 (只需启动一次，后续无需停止)
    // 可以在 User_App_Init 中全局启动，或者在这里判断
    HAL_TIM_Base_Start(&htim5);

    // 2. 发送触发信号 (严格 10us 高电平)
    HAL_GPIO_WritePin(TrigPort, TrigPin, GPIO_PIN_RESET);
    delay_us(2);
    HAL_GPIO_WritePin(TrigPort, TrigPin, GPIO_PIN_SET);
    delay_us(12);
    HAL_GPIO_WritePin(TrigPort, TrigPin, GPIO_PIN_RESET);

    // 3. 等待 Echo 变高 (增加 5ms 超时保护，防止传感器故障卡死主循环)
    uint32_t timeout_cnt = 0;
    while (HAL_GPIO_ReadPin(EchoPort, EchoPin) == GPIO_PIN_RESET) {
        if (++timeout_cnt > 5000) return 0.0f;
        delay_us(1);
    }
    start_time = __HAL_TIM_GET_COUNTER(&htim5);

    // 4. 等待 Echo 变低 (量程限幅：30ms 对应约 5 米)
    while (HAL_GPIO_ReadPin(EchoPort, EchoPin) == GPIO_PIN_SET) {
        if (__HAL_TIM_GET_COUNTER(&htim5) - start_time > 30000) break;
    }
    end_time = __HAL_TIM_GET_COUNTER(&htim5);

    // 5. 计算距离
    // 1MHz 频率下，1个 count = 1us
    // 距离 = (时间 * 0.0343) / 2  => 时间 / 58.3
    float distance = (float)(end_time - start_time) / 58.3f;

    return distance;
}

/**
 * @brief  获取巡线传感器状态
 * @return 0:直行, 1:左偏, 2:右偏, 3:全黑/全白
 */
uint8_t Get_Line_Status(void) {
    // 根据 config.h 中定义的引脚读取状态
    uint8_t left = HAL_GPIO_ReadPin(LINE_L_PORT, LINE_L_PIN);
    uint8_t right = HAL_GPIO_ReadPin(LINE_R_PORT, LINE_R_PIN);

    // 逻辑判定 (假设传感器检测到黑线输出为 1)
    if (left == 0 && right == 0) return 0; // 都在白地上 -> 直行
    if (left == 1 && right == 0) return 1; // 左侧压黑线 -> 向左转修正
    if (left == 0 && right == 1) return 2; // 右侧压黑线 -> 向右转修正
    return 3;                              // 其他异常状态
}
