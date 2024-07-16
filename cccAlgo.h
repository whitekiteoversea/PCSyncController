#ifndef CCCALGO_H
#define CCCALGO_H

#include "pid.h"

typedef struct {
    volatile unsigned int positionA;
    volatile unsigned int positionB;
    volatile short controlSignalA;
    volatile short controlSignalB;
    PIDController pidA;
    PIDController pidB;
    float kp1;
    float kp2;
} CCCCONTROLLER;

float getRelevantPositionA(void);
float getRelevantPositionB(void);
void setControlSignalA(unsigned short controlSignal);
void setControlSignalB(unsigned short controlSignal);
void controlLoop(int posiTaskum);
void controllerInit(void);

void singleMotorPosiTask(unsigned char sendNo, int posiTaskum);
void checkTaskAccomplish(void);

extern CCCCONTROLLER ccc_Control;
extern PIDController posiPIDA;
extern PIDController posiPIDB;

#endif // CCCALGO_H
