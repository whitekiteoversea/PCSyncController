#include "cccAlgo.h"
#include <cmath>
#include <qDebug>

CCCCONTROLLER ccc_Control;
PIDController posiPIDA;
PIDController posiPIDB;
MotionDataCol dataCol;

void controllerInit(void)
{
    PIDController_Init_WorkMode(&ccc_Control.pidA, 1);
    PIDController_Init_WorkMode(&ccc_Control.pidB, 1);

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
    // 计算同步误差
    volatile int syncError = currentPositionA - currentPositionB + SETUP_HIGH_COMPENSATION_UM;  // 位置同步补偿
    ccc_Control.rotateAngle = atan((double)syncError/ZAXIS_DISTANCE); // 计算实时旋转角度

    // 计算同步误差补偿，量纲为um
    short controlOutputA = syncError * ccc_Control.kp1 * (-1);
    short controlOutputB = syncError * ccc_Control.kp2;

    // 更新位置环输出
    PIDController_Update(&ccc_Control.pidA, posiTaskum+controlOutputA, currentPositionA);
    PIDController_Update(&ccc_Control.pidB, posiTaskum+controlOutputB, currentPositionB);

    // 更新该周期控制输出
    setControlSignalA((short)(ccc_Control.pidA.out));
    setControlSignalB((short)(ccc_Control.pidB.out));
}


unsigned char smcSyncTask(void)
{
    unsigned char ret = 0;

    return ret;
}

/* Params:
posiTaskum: 双轴绝对位移
workMode: 速度环/转矩环控制
*/
unsigned char controlLoopWithWorkMode(int posiTaskum, unsigned char workMode)
{
    unsigned char ret = 0;
    // 获取最新位置数据
    unsigned int currentPositionA = getRelevantPositionA();
    unsigned int currentPositionB = getRelevantPositionB();
    short controlOutputA = 0;
    short controlOutputB = 0;
    int initialError[2] = {0, 0};
    volatile int syncError = currentPositionA - currentPositionB + SETUP_HIGH_COMPENSATION_UM;  // 位置同步误差

    qDebug() << "Now SyncTask is Step" << posiTask.taskPeriod << "\n";

    if (posiTask.taskPeriod == 0) {
        posiTask.taskPeriod = 1;
    } else if (posiTask.taskPeriod == 1) { //调平
        initialError[0] = currentPositionA - posiTaskum;
        initialError[1] = currentPositionB - posiTaskum - SETUP_HIGH_COMPENSATION_UM;
        if ((initialError[0] * initialError[1]) >= 0) { // 双机运动方向一致
            // 判断是否需要进行预调平
            if (syncError < MAX_ALLOWED_MECHANICAL_ERROR_UM) {
                // 调平完成，重新设定同步初始位置
                posiTask.relevantInitPosi[0] = getRelevantPositionA();
                posiTask.relevantInitPosi[1] = getRelevantPositionB();
                // 停车进入下一阶段
                ccc_Control.pidA.out = 0;
                ccc_Control.pidB.out = 0;
                setControlSignalA((short)(ccc_Control.pidA.out));
                setControlSignalB((short)(ccc_Control.pidB.out));
                posiTask.taskPeriod = 2;
                return ret;
            }
            // 当存在较大初始误差时，距离近的轴静止等待调平完成
            if (std::abs(initialError[0]) <= std::abs(initialError[1])) {
                ccc_Control.pidA.out = 0;
                singleMotorPosiTask(2, currentPositionA, workMode);
            } else {
                ccc_Control.pidB.out = 0;
                singleMotorPosiTask(1, currentPositionB, workMode);
            }
        } else if ((initialError[0] * initialError[1]) < 0) { // 方向相反，此时无需考虑同步误差，直接下一阶段启动归中
            posiTask.taskPeriod = 4;
        }
    } else if (posiTask.taskPeriod == 2) { // 同步启动
        ccc_Control.rotateAngle = std::atan2(syncError, ZAXIS_DISTANCE)*RAD_DU; // 更新同步旋转角度
#if CCC_ALGO_ENABLE
        if (ccc_Control.taskAccomplishFlag == 1) {
            posiTask.taskPeriod = 3;
            ccc_Control.pidA.out = 0;
            ccc_Control.pidB.out = 0;
            return ret;
        }
        controlOutputA = syncError * ccc_Control.kp1 * (-1);
        controlOutputB = syncError * ccc_Control.kp2;
        // 更新位置环PID输出
        PIDController_Update_WorkMode(&ccc_Control.pidA, posiTaskum+controlOutputA, currentPositionA, workMode);
        PIDController_Update_WorkMode(&ccc_Control.pidB, posiTaskum+controlOutputB, currentPositionB, workMode);
#else
        smcSyncTask();
#endif
    }  else if (posiTask.taskPeriod == 3) { // 同步任务执行完毕停车
        ccc_Control.pidA.out = 0;
        ccc_Control.pidB.out = 0;
    } else if (posiTask.taskPeriod == 4) { // 单独处理无需同步的情况，双机位置环单独运行
        singleMotorPosiTask(1, posiTaskum, workMode);
        singleMotorPosiTask(2, posiTaskum, workMode);
    } else {
        posiTask.taskPeriod = 0;
    }
    // 更新控制输出
    setControlSignalA((short)(ccc_Control.pidA.out));
    setControlSignalB((short)(ccc_Control.pidB.out));
    return ret;
}

// 单机调试
void singleMotorPosiTask(unsigned char sendNo, int posiTaskum, unsigned char workMode)
{
    uint32_t realtimeRelevantPosium = 0;
    if (sendNo == 1) {
        realtimeRelevantPosium = getRelevantPositionA();
        PIDController_Update_WorkMode(&posiPIDA, posiTaskum, realtimeRelevantPosium, workMode);
    } else if (sendNo == 2) {
        realtimeRelevantPosium = getRelevantPositionB();
        PIDController_Update_WorkMode(&posiPIDB, posiTaskum, realtimeRelevantPosium, workMode);
    }
}

//判断位置环控制任务是否完成
// 任务指标：连续1s反馈数据与给定相差都在5%内
unsigned char checkTaskAccomplish(int targetPosiUM, unsigned int returnPosiUM)
{
    unsigned char ret = 0;
    static unsigned short arrivalPeriodCnt = 0;
    int32_t trackErr = targetPosiUM - returnPosiUM;
    // 这里由于速度模式下下发命令为rpm，导致即使将误差率降低，也无法再实际产生调整（rpm<0.1rpm）

    if (std::abs(trackErr-targetPosiUM) <31){ //绝对误差<31um 根据相对较差的机械精度来决定
        arrivalPeriodCnt++;
    } else {
        arrivalPeriodCnt = 0;
    }

    // 持续1s
    if (arrivalPeriodCnt >= (5000/CONTROL_SENSOR_PERIOD_MS)) {
        ret = 1;
    }
    return ret;
}

// 同步过程数据记录
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
