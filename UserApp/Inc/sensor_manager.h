#ifndef __SENSOR_MANAGER_H
#define __SENSOR_MANAGER_H

/* 包含必要的头文件 */
#include "main.h"

/* ==========================================================================
 * 传感器驱动函数声明
 * ========================================================================== */

/**
 * @brief  获取超声波传感器距离
 * @param  TrigPort: GPIO端口 (例如 GPIOB)
 * @param  TrigPin:  GPIO引脚  (例如 GPIO_PIN_0)
 * @param  EchoPort: GPIO端口
 * @param  EchoPin:  GPIO引脚
 * @return 距离 (单位: cm)
 */
float Ultrasonic_GetDistance(GPIO_TypeDef* TrigPort, uint16_t TrigPin, GPIO_TypeDef* EchoPort, uint16_t EchoPin);

/**
 * @brief  获取巡线传感器状态
 * @return 0:直行, 1:左偏, 2:右偏, 3:全白/全黑
 */
uint8_t Get_Line_Status(void);

#endif /* __SENSOR_MANAGER_H */
