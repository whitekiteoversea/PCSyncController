#ifndef FRAMESHEET_H
#define FRAMESHEET_H

#include <QMetaType>
#include <QVariant>

#define uint8_t unsigned char
#define uint16_t unsigned short
#define int16_t short
#define uint32_t unsigned int

#define BufMaxStorageCnt        (6000)
#define SingleShotMaxStorageCnt (BufMaxStorageCnt/2)

#define MAXDELAYLEN             (5)
#define MAXCASNODENUM           (2)

#pragma pack(1)

// Standard CANID
typedef union
{
    struct
    {
        uint32_t MasterOrSlave : 1; 	//主从角色
        uint32_t CTRCode : 5;
        uint32_t NodeGroupID : 5;
        uint32_t Reserve : 21;
    }STDCANID;
    uint32_t Value;
}CANID_STD;

//CANFrame
typedef struct
{
    CANID_STD CANID;
    uint8_t CANData[8];
}CANFrame_STD;

//时间戳报文
typedef struct timeStamp {
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
    uint32_t msec;
}curTime;

//Ethernet报文
/*报文类型有三种：
 * Etype =1: 无预测直接发 帧号+当前PC UTC时间+预测速度
 * Etype =2: NPC预测控制 帧号+预测作用 UTC时间 + 预测速度（需要使用ETH包中的时间来给定发送时间）
 * Etype =4: 时钟同步帧  帧号+当前PC UTC时间
 * Etype =5: 周期获取位置反馈 帧号+当前PC UTC时间+当前查询位置/ 帧号+当前UTC同步时间+最新获取位置信号
 * EType =6: 上传位置同步误差信息 帧号+ 当前UTC同步时间+实时平均误差信息
 * Etype =7: PC向ETH获取时延模拟数据
 * Etype =8: PC向CAS获取事件触发数据
 * EType =0x1F: PC向CAS获取存储位移数据
 *
*/

// ETH下发报文指令类型
#define SPEEDCMD            (0x00)
#define PRESPEED            (0x01)
#define TIMESYNC            (0x02)
#define POSIACQUIRE         (0x03)
#define DATAUPLOAD          (0x1F)

typedef struct
{
    uint32_t EHeader;        //Ethernet帧头
    uint32_t ENum;           //Ethernet帧号
    uint32_t ELen;           //Ethernet长度 Byte
    uint8_t EType;           //报文类型
    uint32_t sendTimeStamp;  //发送时间戳
    uint8_t canEnable[MAXCASNODENUM];      //下发 or 上传数据包含can1节点数据
    CANFrame_STD canCmd[MAXCASNODENUM];      //CAN节点1 报文
    uint32_t EChecksum;
    uint32_t FrameTailer;
}EthControlFrame;

typedef struct
{
    uint32_t EHeader;        //Ethernet帧头
    uint32_t ENum;           //Ethernet帧号
    uint32_t ELen;           //Ethernet长度 Byte
    uint8_t EType;           //报文类型
    CANFrame_STD canpack;
    uint32_t FrameTailer;
}EthControlFrameSingleCAS;

typedef struct
{
    uint32_t EHeader;         //Ethernet帧头
    uint32_t ENum;            //Ethernet帧号
    uint32_t ELen;           //Ethernet长度 Byte
    uint8_t EType;           //报文类型
    uint8_t ETotalPackNum;   //总CAN包数
    uint8_t can_preLen[MAXCASNODENUM];     //CAN预测长度
    uint32_t sendTimeStamp;  //发送时间戳
    uint8_t canEnable[MAXCASNODENUM];      //下发 or 上传数据包含can节点数据
    CANFrame_STD canPreCmd[MAXCASNODENUM][MAXDELAYLEN];  //CAN1预测命令
    uint32_t EChecksum;
    uint32_t FrameTailer;
}EthPredFrame;

//时钟同步帧
typedef struct {
    uint32_t EHeader;         //Ethernet帧头
    uint32_t ENum;            //Ethernet帧号
    uint32_t ELen;            //Ethernet长度 Byte
    uint8_t EType;            //报文类型
    uint32_t timeSync;
    uint16_t EChecksum;
    uint16_t FrameTailer;
}EtherFrame;

//预测给定速度序列
typedef struct {
    uint32_t preTime[15];       //ms
    short preGivenSpeed[15];    //rpm
}EthUARRPRE;

/* 报文协议定义
ETH: 0x01 位置查询
     0x02 速度给定
     0x1E 时间同步
     0x1F 存储数据读取
(存疑)

CAS: 0x01 位置查询
     0x01 速度给定
     0x04 时间同步
     0x0A 存储数据读取
 */

#define CANSpeedCmd                     (1)
#define CANSpeedPreCmd                  (2)
#define CANOperationModeCmd             (3)
#define CANTimeSyncCmd                  (4)
#define CANPisiAcquireCmd               (5)
#define CANTimeSyncErrorCalCmd          (6)
#define CANLocalPITestCmd               (7)
#define CANDriverInfoAcquire            (10)


//串口通信帧格式（其实没啥用，仅作为编程参考）
typedef struct serialData
{
    unsigned char FrameHeader_1;
    unsigned char FrameHeader_2;
    unsigned char reCVSource;
    unsigned char timeStamp[4];
    unsigned char feedbackSpeed[2];
    unsigned char sumCheck;
    unsigned char frameTailer;
}SerialFrame;

//自定义数据传递结构
typedef struct speedFormat
{
    unsigned short curSpeed;    //反馈速度
    uint32_t timeStamp;         //ms时间戳
}speedUpdateFormat;
Q_DECLARE_METATYPE(speedUpdateFormat);

typedef struct {
    uint16_t frameNum;
    uint32_t sampleTimeStamp;
    int32_t pulseCnt;
}feedbackData;
Q_DECLARE_METATYPE(feedbackData);

typedef struct
{
    uint32_t recvTimeStamp;
    uint32_t referSig;
    int16_t avgRecord;
} avgPosiRecord;

#pragma pack()

uint16_t WordCombine(uint8_t byte1, uint8_t byte2);

extern volatile unsigned char curAlgoMode;

#endif // FRAMESHEET_H
