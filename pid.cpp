#include "pid.h"
#include "FrameSheet.h"
#include <QDebug>

// 位置环缩放系数
#define POSILOOPEXPANDCOFF   (1000)
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
    pid->limMaxInt = 10000;
    pid->limMinInt = -10000;

    pid->T = 0.005; // 5ms周期

    pid->Kp = 30;
    pid->Ki= 0; // 位置环默认不进行积分
    pid->Kd = 0;

    pid->out = 0.0f;
}

void PIDController_Init_WorkMode(PIDController *pid, unsigned char workMode)
{
    /* Clear controller variables */
    pid->integrator = 0.0f;
    pid->prevError  = 0.0f;

    pid->differentiator  = 0.0f;
    pid->prevMeasurement = 0.0f;

    if (workMode == 0x03) {
        // 控制输出阈值
        pid->limMin= -10000; // 0.1rpm
        pid->limMax= 10000;  // 0.1rpm

        // 积分上限
        pid->limMaxInt = 10000;
        pid->limMinInt = -10000;

        pid->Ki= 0; // 位置环默认不进行积分
    } else if (workMode == 0x04) {
        // 控制输出阈值
        pid->limMin= -2800; // 0.001Nm
        pid->limMax= 2800;  //

        // 积分上限
        pid->limMaxInt = 2800;
        pid->limMinInt = -2800;

        pid->Ki= 0.0; // 转矩换默认进行积分
    }

    pid->T = 0.005; // 5ms周期
    pid->Kp = 30;
    pid->Kd = 0;

    pid->out = 0.0f;

    pid->recCnt = 0;
    pid->onlyShowOnceFlag = 0;
}


// 输入 参考位移 um 输出 rpm
float PIDController_Update(PIDController *pid, float setpoint, float measurement) {
    float error = setpoint - measurement;
    float proportional = pid->Kp * error;
    pid->integrator = pid->integrator + 0.5f * pid->Ki * pid->T * (error + pid->prevError); // 梯形积分

    /* Anti-wind-up via integrator clamping */
    if (pid->integrator > pid->limMaxInt) {
        pid->integrator = pid->limMaxInt;
    } else if (pid->integrator < pid->limMinInt) {
        pid->integrator = pid->limMinInt;
    }

//    pid->differentiator = -(2.0f * pid->Kd * (measurement - pid->prevMeasurement)	/* Note: derivative on measurement, therefore minus sign in front of equation! */
//                        + (2.0f * pid->tau - pid->T) * pid->differentiator)
//                        / (2.0f * pid->tau + pid->T);

//    pid->out = proportional + pid->integrator + pid->differentiator;
    pid->out = proportional + pid->integrator;
    //pid->out  = pid->out / POSILOOPEXPANDCOFF;

    if (pid->out > pid->limMax) {
        pid->out = pid->limMax;
    } else if (pid->out < pid->limMin) {
        pid->out = pid->limMin;
    }

    // qDebug() << "PID->out" << QString::number(pid->out, 'g', 3) << "\n";
    pid->prevError       = error;
    pid->prevMeasurement = measurement;
    return pid->out;
}


short PIDController_Update_WorkMode(PIDController *pid, float setpoint, float measurement, unsigned char workMode)
{
    float error = setpoint - measurement;
    float proportional = pid->Kp * error;
    pid->integrator = pid->integrator + 0.5f * pid->Ki * pid->T * (error + pid->prevError);

    /* Anti-wind-up via integrator clamping */
    if (pid->integrator > pid->limMaxInt) {
        pid->integrator = pid->limMaxInt;
    } else if (pid->integrator < pid->limMinInt) {
        pid->integrator = pid->limMinInt;
    }

//    pid->differentiator = -(2.0f * pid->Kd * (measurement - pid->prevMeasurement)	/* Note: derivative on measurement, therefore minus sign in front of equation! */
//                            + (2.0f * pid->tau - pid->T) * pid->differentiator)
//                          / (2.0f * pid->tau + pid->T);

//    pid->out = proportional + pid->integrator + pid->differentiator;
      pid->out = proportional + pid->integrator;

    if (workMode == 0x03) {
        pid->out  = pid->out / POSILOOPEXPANDCOFF;
    } else if (workMode == 0x04) {
        pid->out  = pid->out / TORQUELOOPEXPANDCOFF;
    }

    // 限幅
    if (pid->out > pid->limMax) {
        pid->out = pid->limMax;
        qDebug() << "pid->limMax: " << pid->limMax << " \n";
    } else if (pid->out < pid->limMin) {
        pid->out = pid->limMin;
        qDebug() << "pid->limMin: " << pid->limMin << " \n";
    }
    pid->prevError       = error;
    pid->prevMeasurement = measurement;

    return (short)(pid->out);
}
