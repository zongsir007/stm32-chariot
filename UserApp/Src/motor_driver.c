#include "motor_driver.h"
#include "config.h"
#include <math.h>

Motor_Status_t Motor_Stats[4];

// 过滤器配置：接收所有 ID，方便调试回传
void CAN_Filter_Config(void) {
    CAN_FilterTypeDef filter;
    filter.FilterBank = 0;
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;
    filter.FilterIdHigh = 0x0000;
    filter.FilterIdLow = 0x0000;
    filter.FilterMaskIdHigh = 0x0000;
    filter.FilterMaskIdLow = 0x0000;
    filter.FilterFIFOAssignment = CAN_RX_FIFO0;
    filter.FilterActivation = ENABLE;
    HAL_CAN_ConfigFilter(&CAN_BUS_HANDLE, &filter);
}

/**
 * @brief 发送速度控制指令给单个张大头电机
 * @param std_id 电机拨码 ID (通常 1, 2, 3, 4)
 * @param rpm 目标转速
 */
void Motor_Send_Control_Single(uint16_t std_id, int16_t rpm) {
    CAN_TxHeaderTypeDef tx_header;
    uint8_t tx_data[8] = {0};
    uint32_t mailbox;

    if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
            return;
        }

    tx_header.StdId = std_id;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 8;

    // --- 张大头标准协议：速度模式指令 (0xF3) ---
    tx_data[0] = 0xF3;                      // 写入速度指令码
    tx_data[1] = (rpm >= 0) ? 0x00 : 0x01; // 方向位：0正转，1反转

    uint16_t abs_rpm = (rpm >= 0) ? rpm : -rpm;
    tx_data[2] = (uint8_t)(abs_rpm >> 8);   // 速度高位
    tx_data[3] = (uint8_t)(abs_rpm & 0xFF); // 速度低位
    tx_data[4] = 0x00;                      // 加速度 (0代表默认/最快)

    HAL_CAN_AddTxMessage(&CAN_BUS_HANDLE, &tx_header, tx_data, &mailbox);
}

/**
 * @brief 适配你之前运动逻辑的组控制函数
 */
void Motor_Send_Control_Group(int16_t v1, int16_t v2, int16_t v3, int16_t v4) {
    // 假设你的 ID 拨码分别是 1, 2, 3, 4
    Motor_Send_Control_Single(1, v1);
    Motor_Send_Control_Single(2, v2);
    Motor_Send_Control_Single(3, v3);
    Motor_Send_Control_Single(4, v4);
}

/**
 * @brief 停止所有电机
 */
void Motor_Stop_All(void) {
    Motor_Send_Control_Group(0, 0, 0, 0);
}

/**
 * @brief 使能电机（部分步进电机上电默认锁死或释放，需要使能指令）
 * 指令码通常为 0xF3 且速度为0，或者特定的 0xF1 释放指令。
 */
void Motor_Enable_All(void) {
    // 这里发送一个速度为0的指令通常能激活电机
    Motor_Send_Control_Group(0, 0, 0, 0);
}

/**
 * @brief 反馈解析：张大头回传通常包含当前位置和速度
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK) {
        // 假设反馈 ID 为 0x01-0x04
        if (rx_header.StdId >= 1 && rx_header.StdId <= 4) {
            uint8_t idx = rx_header.StdId - 1;
            // 协议不同，回传位置可能不同，这里仅作占位
            Motor_Stats[idx].rpm = (int16_t)(rx_data[2] << 8 | rx_data[3]);
        }
    }
}
