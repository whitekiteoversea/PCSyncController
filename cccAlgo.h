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

    unsigned char taskAccomplishFlag;

    float rotateAngle;
} CCCCONTROLLER;

float getRelevantPositionA(void);
float getRelevantPositionB(void);
void setControlSignalA(unsigned short controlSignal);
void setControlSignalB(unsigned short controlSignal);
void controlLoop(int posiTaskum);
void controllerInit(unsigned char workMode);

void singleMotorPosiTask(unsigned char sendNo, int posiTaskum, unsigned char workMode);
void DualMotorPosiTask(CCCCONTROLLER *pCon, unsigned char sendNo, int posiTaskum, unsigned char workMode);
unsigned char checkTaskAccomplish(int targetPosiUM, unsigned int returnPosiUM);

unsigned char controlLoopWithWorkMode(int posiTaskum, unsigned char workMode);

// SMC
unsigned char smcSyncTask(void);


// 数据统计函数
void dataCollection(void);

extern CCCCONTROLLER ccc_Control;
extern PIDController posiPIDA;
extern PIDController posiPIDB;

extern MotionDataCol dataCol;

#endif // CCCALGO_H
