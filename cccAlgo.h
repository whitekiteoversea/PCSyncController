#ifndef CCCALGO_H
#define CCCALGO_H

#include "pid.h"
#include "FrameSheet.h"

typedef struct {
    volatile unsigned int positionA;
    volatile unsigned int positionB;
    volatile short controlSignalA;
    volatile short controlSignalB;
    PIDController pidA;
    PIDController pidB;
    float kp1;
    float kp2;

    float rotateAngle;
} CCCCONTROLLER;

float getRelevantPositionA(void);
float getRelevantPositionB(void);
void setControlSignalA(unsigned short controlSignal);
void setControlSignalB(unsigned short controlSignal);
void controlLoop(int posiTaskum);
void controllerInit(void);

void singleMotorPosiTask(unsigned char sendNo, int posiTaskum);
unsigned char checkTaskAccomplish(unsigned int targetPosiUM, unsigned int returnPosiUM);

void controlLoopWithWorkMode(int posiTaskum, unsigned char workMode);

// 数据统计函数
void dataCollection(void);

extern CCCCONTROLLER ccc_Control;
extern PIDController posiPIDA;
extern PIDController posiPIDB;

extern MotionDataCol dataCol;

#endif // CCCALGO_H
