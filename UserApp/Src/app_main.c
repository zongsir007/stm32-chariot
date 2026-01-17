#include "config.h"
#include "motor_driver.h"
#include "sensor_manager.h"
#include "motion_control.h"

/* ==========================================================================
 * 1. 物理标定数据 (仅需在此处修改，全篇自动生效)
 * ========================================================================== */
#define CALIB_LINEAR_SPEED  1500  // 直行参数
#define CM_PER_1000MS       40.0f // 1500速度下，1秒走多少cm

#define CALIB_ROTATE_SPEED  1000  // 转向参数
#define MS_PER_90_DEG       1200  // 1000速度下，转90度用时ms

/* --- 换算宏：将物理尺寸转换为毫秒时间 --- */
// 算法：(目标距离 / 每毫秒走的距离)
#define DIST_CM(cm)    ((uint32_t)((cm) * (1000.0f / CM_PER_1000MS)))
#define ANGLE_DEG(deg) ((uint32_t)((deg) * (MS_PER_90_DEG / 90.0f)))

/* ==========================================================================
 * 2. 全局监控变量
 * ========================================================================== */
float current_dist = 100.0f;
uint32_t g_elapsed = 0;
uint32_t g_step = 0;

void User_App_Init(void) {
    CAN_Filter_Config();
    HAL_CAN_Start(&CAN_BUS_HANDLE);
    HAL_CAN_ActivateNotification(&CAN_BUS_HANDLE, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void User_App_Task(void) {
    static uint32_t last_ctrl_tick = 0;
    static uint32_t last_us_tick = 0;
    static uint32_t start_time = 0;

    uint32_t now = HAL_GetTick();
    if (start_time == 0) start_time = now;

    // 1. 传感器任务 (防止阻塞)
    if (now - last_us_tick >= 60) {
        last_us_tick = now;
        current_dist = 100.0f; // 实际测试时换回 Ultrasonic_GetDistance
        //current_dist = Ultrasonic_GetDistance;
    }

    // 2. 运动控制逻辑
    if (now - last_ctrl_tick >= 10) {
        last_ctrl_tick = now;

        if (current_dist < SAFE_DISTANCE) {
            Chassis_Stop();
            return;
        }

        uint32_t elapsed = now - start_time;
        g_elapsed = elapsed;

        /* --- 定义步长变量 (用于乘法表示) --- */
        const uint32_t L10 = DIST_CM(10);
        const uint32_t T90 = ANGLE_DEG(90);

        /* --- 动作阶段时间点计算 (累加法) --- */
        uint32_t t1  = 4 * L10;                                   // 40cm
        uint32_t t2  = t1 + 1 * T90;                              // 出门
        uint32_t t3  = t2 + 20 * L10;                             // 直行200cm
        uint32_t t4  = t3 + 1 * T90;                              // 转角1
        uint32_t t5  = t4 + 20 * L10;                             // 直行200cm
        uint32_t t6  = t5 + 1 * T90;                              // 转角2
        uint32_t t7  = t6 + 20 * L10;                             // 直行200cm
        uint32_t t8  = t7 + 1 * T90;                              // 转角3
        uint32_t t9  = t8 + 12 * L10;                             // 到得分区120cm
        uint32_t t10 = t9 + 1 * T90;                              // 转向得分区
        uint32_t t11 = t10+ 4* L10;								  // 进入得分区

        /* === 执行开环剧本 === */
        if (elapsed < t1) {
            Move_Forward(CALIB_LINEAR_SPEED);
            g_step = 1;
        }
        else if (elapsed < t2) {
            Rotate_Right(CALIB_ROTATE_SPEED);
            g_step = 2;
        }
        else if (elapsed < t3) {
            Move_Forward(CALIB_LINEAR_SPEED);
            g_step = 3;
        }
        else if (elapsed < t4) {
            Rotate_Left(CALIB_ROTATE_SPEED);
            g_step = 4;
        }
        else if (elapsed < t5) {
            Move_Forward(CALIB_LINEAR_SPEED);
            g_step = 5;
        }
        else if (elapsed < t6) {
            Rotate_Left(CALIB_ROTATE_SPEED);
            g_step = 6;
        }
        else if (elapsed < t7) {
            Move_Forward(CALIB_LINEAR_SPEED);
            g_step = 7;
        }
        else if (elapsed < t8) {
            Rotate_Left(CALIB_ROTATE_SPEED);
            g_step = 8;
        }
        else if (elapsed < t9) {
            Move_Forward(CALIB_LINEAR_SPEED);
            g_step = 9;
        }
        else if (elapsed < t10) {
            Rotate_Right(CALIB_ROTATE_SPEED);
            g_step = 10;
        }
        else if (elapsed < t11) {
            Move_Forward(CALIB_LINEAR_SPEED);
            g_step = 11;
        }
        else {
            Chassis_Stop();
            g_step = 99;
        }
    }
}
