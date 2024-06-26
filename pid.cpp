#include "pid.h"

#define OutPut_UP_LIMIT 4000
#define OutPut_DW_LIMIT -4000

void PIDController_Init(PIDController *pid) {

    /* Clear controller variables */
    pid->integrator = 0.0f;
    pid->prevError  = 0.0f;

    pid->differentiator  = 0.0f;
    pid->prevMeasurement = 0.0f;

    pid->limMin= -1800;
    pid->limMax= 1800;

    pid->limMaxInt = 10000;
    pid->limMinInt = -10000;

    pid->T = 0.05;

    pid->Kp = 3;
    pid->Ki= 0; // 位置环默认不进行积分
    pid->Kd = 0;

    pid->out = 0.0f;

}

float PIDController_Update(PIDController *pid, float setpoint, float measurement) {

    /*
    * Error signal
    */
    float error = setpoint - measurement;


    /*
    * Proportional
    */
    float proportional = pid->Kp * error;


    /*
    * Integral
    */
    pid->integrator = pid->integrator + 0.5f * pid->Ki * pid->T * (error + pid->prevError);

    /* Anti-wind-up via integrator clamping */
    if (pid->integrator > pid->limMaxInt) {

        pid->integrator = pid->limMaxInt;

    } else if (pid->integrator < pid->limMinInt) {

        pid->integrator = pid->limMinInt;

    }


    /*
    * Derivative (band-limited differentiator)
    */

    pid->differentiator = -(2.0f * pid->Kd * (measurement - pid->prevMeasurement)	/* Note: derivative on measurement, therefore minus sign in front of equation! */
                        + (2.0f * pid->tau - pid->T) * pid->differentiator)
                        / (2.0f * pid->tau + pid->T);


    /*
    * Compute output and apply limits
    */
    pid->out = proportional + pid->integrator + pid->differentiator;

    pid->out  = (pid->out/2060)*30; //转换为rpm

    if (pid->out > pid->limMax) {

        pid->out = pid->limMax;

    } else if (pid->out < pid->limMin) {

        pid->out = pid->limMin;

    }

    /* Store error and measurement for later use */
    pid->prevError       = error;
    pid->prevMeasurement = measurement;

    /* Return controller output */
    return pid->out;

}
