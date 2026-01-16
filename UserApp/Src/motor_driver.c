#include "motor_driver.h"
#include "config.h"

Motor_Status_t Motor_Stats[4];
PID_TypeDef    Motor_PID[4];

void PID_Init(void) {
    for (int i = 0; i < 4; i++) {
        Motor_PID[i].kp = 1.5f;   // 初始值，若起步无力可调大至 3.0~5.0
        Motor_PID[i].ki = 0.1f;
        Motor_PID[i].kd = 0.01f;
        Motor_PID[i].max_output = 16384;
        Motor_PID[i].max_integral = 2000;
        Motor_PID[i].integral = 0;
        Motor_PID[i].last_error = 0;
    }
}

float PID_Compute(PID_TypeDef *pid, float target, float current) {
    pid->error = target - current;
    pid->integral += pid->error;

    // 积分限幅
    if (pid->integral > pid->max_integral)  pid->integral = pid->max_integral;
    if (pid->integral < -pid->max_integral) pid->integral = -pid->max_integral;

    pid->output = (pid->kp * pid->error) + (pid->ki * pid->integral) + (pid->kd * (pid->error - pid->last_error));
    pid->last_error = pid->error;

    // 输出限幅
    if (pid->output > pid->max_output)  pid->output = pid->max_output;
    if (pid->output < -pid->max_output) pid->output = -pid->max_output;

    return pid->output;
}

void Motor_Send_Control_Group(int16_t v1, int16_t v2, int16_t v3, int16_t v4) {
    Motor_Stats[0].target_rpm = v1;
    Motor_Stats[1].target_rpm = v2;
    Motor_Stats[2].target_rpm = v3;
    Motor_Stats[3].target_rpm = v4;

    // 计算 PID 输出
    int16_t out[4];
    for(int i=0; i<4; i++) {
        out[i] = (int16_t)PID_Compute(&Motor_PID[i], Motor_Stats[i].target_rpm, Motor_Stats[i].rpm);
    }

    CAN_TxHeaderTypeDef tx_header;
    uint8_t tx_data[8];
    uint32_t mailbox;

    tx_header.StdId = 0x200;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 8;

    // 填充 4 个电机的电流控制数据
    for(int i=0; i<4; i++) {
        tx_data[i*2]   = (uint8_t)(out[i] >> 8);
        tx_data[i*2+1] = (uint8_t)(out[i] & 0xFF);
    }
    HAL_CAN_AddTxMessage(&CAN_BUS_HANDLE, &tx_header, tx_data, &mailbox);
}

void Motor_Stop_All(void) {
    for(int i=0; i<4; i++) Motor_PID[i].integral = 0; // 清除累积误差
    Motor_Send_Control_Group(0, 0, 0, 0);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK) {
        if (rx_header.StdId >= 0x201 && rx_header.StdId <= 0x204) {
            uint8_t idx = rx_header.StdId - 0x201;
            // 大头电机通常回传数据 Data[2,3] 为转速
            Motor_Stats[idx].rpm = (int16_t)(rx_data[2] << 8 | rx_data[3]);
        }
    }
}
