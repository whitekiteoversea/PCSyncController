#include "pid.h"
#include "FrameSheet.h"

// 位置环缩放系数
#define POSILOOPEXPANDCOFF   (100000)
#define TORQUELOOPEXPANDCOFF (2800)

void PIDController_Init(PIDController *pid) {

    /* Clear controller variables */
    pid->integrator = 0.0f;
    pid->prevError  = 0.0f;

    pid->differentiator  = 0.0f;
    pid->prevMeasurement = 0.0f;

    // 控制输出阈值
    pid->limMin= -1000; // rpm
    pid->limMax= 1000;  // rpm

    // 积分上限
    pid->limMaxInt = 10000000;
    pid->limMinInt = -10000000;

    pid->T = 0.005; // 5ms周期

    pid->Kp = 2;
    pid->Ki= 0; // 位置环默认不进行积分
    pid->Kd = 0;

    pid->out = 0.0f;
}

void PIDController_Init_WorkMode(PIDController *pid, unsigned char workMode) {

    /* Clear controller variables */
    pid->integrator = 0.0f;
    pid->prevError  = 0.0f;

    pid->differentiator  = 0.0f;
    pid->prevMeasurement = 0.0f;

    if (workMode == 0x03) {
        // 控制输出阈值
        pid->limMin= -1000; // rpm
        pid->limMax= 1000;  // rpm

        // 积分上限
        pid->limMaxInt = 10000000;
        pid->limMinInt = -10000000;

        pid->Ki= 0; // 位置环默认不进行积分
    } else if (workMode == 0x04) {
        // 控制输出阈值
        pid->limMin= -2800; // 0.001Nm
        pid->limMax= 2800;  // rpm

        // 积分上限
        pid->limMaxInt = 2800;
        pid->limMinInt = -2800;

        pid->Ki= 0.01; // 转矩换默认进行积分
    }

    pid->T = 0.005; // 5ms周期
    pid->Kp = 2;
    pid->Kd = 0;

    pid->out = 0.0f;
}


// 输入 参考位移 um 输出 rpm
float PIDController_Update(PIDController *pid, float setpoint, float measurement) {
    float error = setpoint - measurement;
    float proportional = pid->Kp * error;
    pid->integrator = pid->integrator + 0.5f * pid->Ki * pid->T * (error + pid->prevError);

    /* Anti-wind-up via integrator clamping */
    if (pid->integrator > pid->limMaxInt) {
        pid->integrator = pid->limMaxInt;
    } else if (pid->integrator < pid->limMinInt) {
        pid->integrator = pid->limMinInt;
    }

    pid->differentiator = -(2.0f * pid->Kd * (measurement - pid->prevMeasurement)	/* Note: derivative on measurement, therefore minus sign in front of equation! */
                        + (2.0f * pid->tau - pid->T) * pid->differentiator)
                        / (2.0f * pid->tau + pid->T);

    pid->out = proportional + pid->integrator + pid->differentiator;

    // 2024.07.16
    // 需要进行比例缩放到实际允许的控制输出范畴，比如这里已设定下位机PMSM速度环参考输入 [-1000,1000] rpm
    // 以尽量避免使长期超过上下限，此时会导致环路控制实际开环，控制失效
    // 无积分环节时上下限估算较为简单 Kp*|e|max = Kp*50000000
    // PI时为 Kp*|e|max + Isum * Ki = Kp*50000000 + 10000000 *Ki
    // P模式下最大 2*50000000/100000=1000 PI模式下最大 1200
    // 实际上由于e一般很小(总任务距离只有最多400mm, 大多数时候跑200-300mm，即便缩放前也很难超限)
    // PI模式下同样由于总行程有限，I的积分累积也有限，这里设置为最大行程的20%,即100mm
    pid->out  = pid->out / POSILOOPEXPANDCOFF;

    if (pid->out > pid->limMax) {
        pid->out = pid->limMax;
    } else if (pid->out < pid->limMin) {
        pid->out = pid->limMin;
    }
    pid->prevError       = error;
    pid->prevMeasurement = measurement;
    return pid->out;
}


short PIDController_Update_WorkMode(PIDController *pid, float setpoint, float measurement, unsigned char workMode)
{
    float error = setpoint - measurement;
    float proportional = pid->Kp * error;
    pid->integrator = pid->integrator + 0.5f * pid->Ki * pid->T * (error + pid->prevError); // 这里的0.5是？

    /* Anti-wind-up via integrator clamping */
    if (pid->integrator > pid->limMaxInt) {
        pid->integrator = pid->limMaxInt;
    } else if (pid->integrator < pid->limMinInt) {
        pid->integrator = pid->limMinInt;
    }

    pid->differentiator = -(2.0f * pid->Kd * (measurement - pid->prevMeasurement)	/* Note: derivative on measurement, therefore minus sign in front of equation! */
                            + (2.0f * pid->tau - pid->T) * pid->differentiator)
                          / (2.0f * pid->tau + pid->T);

    pid->out = proportional + pid->integrator + pid->differentiator;

    if (workMode == 0x03) {
        pid->out  = pid->out / POSILOOPEXPANDCOFF;
    } else if (workMode == 0x04) {
        pid->out  = pid->out / TORQUELOOPEXPANDCOFF;
    }

    if (pid->out > pid->limMax) {
        pid->out = pid->limMax;
    } else if (pid->out < pid->limMin) {
        pid->out = pid->limMin;
    }
    pid->prevError       = error;
    pid->prevMeasurement = measurement;
    return (short)(pid->out);
}
