#include "cccAlgo.h"
#include <cmath>

CCCCONTROLLER ccc_Control;
PIDController posiPIDA;
PIDController posiPIDB;

MotionDataCol dataCol;

void controllerInit(void)
{
    PIDController_Init(&ccc_Control.pidA);
    PIDController_Init(&ccc_Control.pidB);
    ccc_Control.kp1 = 0.4;
    ccc_Control.kp2 = 0.4;
}

float getRelevantPositionA() {
    return (laFData_CH[0].feedbackPosium - LEFT_START_POSI);
}

float getRelevantPositionB() {
    return (laFData_CH[1].feedbackPosium - RIGHT_START_POSI);
}

// 更新控制输出的函数
void setControlSignalA(short controlSignal) {
    ccc_Control.controlSignalA = controlSignal;
}

void setControlSignalB(short controlSignal) {
    ccc_Control.controlSignalB = controlSignal;
}

void controlLoop(int posiTaskum) {
    // 获取最新位置数据
    unsigned int currentPositionA = getRelevantPositionA();
    unsigned int currentPositionB = getRelevantPositionB();
    // 控制输出
    short motorSpeed_rpm[2] = {0};

    // 计算同步误差
    volatile int syncError = currentPositionA - currentPositionB + SETUP_HIGH_COMPENSATION_US;  // 位置同步补偿
    ccc_Control.rotateAngle = atan((double)syncError/ZAXIS_DISTANCE);

    // 计算同步误差补偿，量纲为um
    short controlOutputA = syncError * ccc_Control.kp1 * (-1);
    short controlOutputB = syncError * ccc_Control.kp2;

    // 更新位置环输出
    PIDController_Update(&ccc_Control.pidA, posiTaskum+controlOutputA, currentPositionA);
    PIDController_Update(&ccc_Control.pidB, posiTaskum+controlOutputB, currentPositionB);

    // 更新该周期控制输出
    setControlSignalA(motorSpeed_rpm[0]);
    setControlSignalB(motorSpeed_rpm[1]);
}

// 单机调试

void singleMotorPosiTask(unsigned char sendNo, int posiTaskum)
{
    uint32_t realtimeRelevantPosium = 0;
    if (sendNo == 1) {
        realtimeRelevantPosium = getRelevantPositionA();
        PIDController_Update(&posiPIDA, posiTaskum, realtimeRelevantPosium);
    } else if (sendNo == 2) {
        realtimeRelevantPosium = getRelevantPositionB();
        PIDController_Update(&posiPIDB, posiTaskum, realtimeRelevantPosium);
    }
}

//判断位置环控制任务是否完成
// 任务指标：连续1s反馈数据与给定相差都在2%内
unsigned char checkTaskAccomplish(unsigned int targetPosiUM, unsigned int returnPosiUM)
{
    unsigned char ret = 0;
    static uint32_t arrivalPeriodCnt = 0;
    int32_t trackErr = targetPosiUM - returnPosiUM;
    if (((float)(std::abs(trackErr))/targetPosiUM) <= 0.02) {
        arrivalPeriodCnt++;
    } else {
        arrivalPeriodCnt = 0;
    }
    if (arrivalPeriodCnt >= (1000/CONTROL_SENSOR_PERIOD_MS)) {
        ret = 1;
    }
    return ret;
}

void dataCollection(void)
{
    dataCol.TaskTimeMS += CONTROL_SENSOR_PERIOD_MS;
    if (dataCol.rotateAngle_MAX < ccc_Control.rotateAngle) {
        dataCol.rotateAngle_MAX = ccc_Control.rotateAngle;
    }
    if (dataCol.rotateAngle_MIN > ccc_Control.rotateAngle) {
        dataCol.rotateAngle_MIN = ccc_Control.rotateAngle;
    }
    dataCol.rotateAngle_ABS_MAX = ((dataCol.rotateAngle_MAX+dataCol.rotateAngle_MIN) > 0.0) ? abs(dataCol.rotateAngle_MAX) : abs(dataCol.rotateAngle_MIN);
}