#ifndef PID_H
#define PID_H

typedef struct {
    /* Controller gains */
    float Kp;
    float Ki;
    float Kd;

    /* Derivative low-pass filter time constant */
    float tau;

    /* Output limits */
    float limMin;
    float limMax;

    /* Integrator limits */
    float limMinInt;
    float limMaxInt;

    /* Sample time (in seconds) */
    float T;

    /* Controller "memory" */
    float integrator;
    float prevError;			/* Required for integrator */
    float differentiator;
    float prevMeasurement;		/* Required for differentiator */

    /* Controller output */
    float out;

    // record Data
    volatile unsigned int recCnt;
    volatile unsigned int onlyShowOnceFlag;

}PIDController;

void  PIDController_Init(PIDController *pid);
float PIDController_Update(PIDController *pid, float setpoint, float measurement);

void PIDController_Init_WorkMode(PIDController *pid, unsigned char workMode);
short PIDController_Update_WorkMode(PIDController *pid, float setpoint, float measurement, unsigned char workMode);

#endif // PID_H
