#include "config.h"
#include "motor_driver.h"
#include "sensor_manager.h"
#include "servo_control.h"
#include "motion_control.h" // 修复点 1: 必须包含此头文件以识别 Move_Forward 等函数

/* 全局变量：用于在 Debug 模式下观察数据 */
float current_dist = 100.0f;
uint8_t line_state = 0;

/**
 * @brief  用户应用初始化
 */
extern TIM_HandleTypeDef htim2;

void User_App_Init(void) {
    // 1. 启动 CAN (带过滤器)
    CAN_Filter_Config();
    HAL_CAN_Start(&CAN_BUS_HANDLE);
    HAL_CAN_ActivateNotification(&CAN_BUS_HANDLE, CAN_IT_RX_FIFO0_MSG_PENDING);

    // 2. 启动 4 路舵机 PWM
    HAL_TIM_PWM_Start(&SERVO_TIM_HANDLE, SERVO_CH_1);
    HAL_TIM_PWM_Start(&SERVO_TIM_HANDLE, SERVO_CH_2);
    HAL_TIM_PWM_Start(&SERVO_TIM_HANDLE, SERVO_CH_3);
    HAL_TIM_PWM_Start(&SERVO_TIM_HANDLE, SERVO_CH_4);

    // 3. 将所有舵机归位 (90度)
    Servo_SetAngle(SERVO_CH_1, 90.0f);
    Servo_SetAngle(SERVO_CH_2, 90.0f);

    // 4. 启动PID
    PID_Init();
}

/**
 * @brief  用户主逻辑任务 (由 main.c 循环调用)
 */
void User_App_Task(void) {
    static uint32_t last_pid_tick = 0;
    static uint32_t last_us_tick = 0;
    uint32_t now = HAL_GetTick();

    /* ==========================================================
     * 任务 1：运动控制 (10ms 周期)
     * ========================================================== */
    if (now - last_pid_tick >= PID_PERIOD_MS) {
        last_pid_tick = now;
        line_state = Get_Line_Status();

        // 避障逻辑判断
        if (current_dist > 0.1f && current_dist < SAFE_DISTANCE) {
            Chassis_Stop(); // 发现障碍物停止
        }
        else {
            switch (line_state) {
                case 0: // 直行
                    Move_Forward(1500);
                    break;
                case 1: // 左偏，需要向左转修正
                    Chassis_Drive(1500, 500);
                    break;
                case 2: // 右偏，需要向右转修正
                    Chassis_Drive(1500, -500);
                    break;
                default:
                    Chassis_Stop();
                    break;
            }
        }
    } // 任务 1 结束

    /* ==========================================================
     * 任务 2：超声波测距 (60ms 周期)
     * ========================================================== */
    if (now - last_us_tick >= 60) {
        last_us_tick = now;
        // 获取当前距离
        current_dist = Ultrasonic_GetDistance(US1_TRIG_PORT, US1_TRIG_PIN, US1_ECHO_PORT, US1_ECHO_PIN);
    }
} // 函数结束
