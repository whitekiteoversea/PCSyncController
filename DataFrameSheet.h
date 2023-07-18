#ifndef ETHERNETFRAMESHEET_H
#define ETHERNETFRAMESHEET_H

#define uint8_t unsigned char
#define uint16_t unsigned short
#define int16_t short
#define uint32_t unsigned int

//电机连接状态
extern bool statusflag1;
extern bool statusflag2;

#define CAN1_ID 384
#define CAN2_ID 896

//extern uint32_t Node_CAN_ID_1;
//extern uint32_t Node_CAN_ID_2;

//Standard CANID
typedef union
{
    struct //从高到低
    {
        uint32_t MasterSlaveID : 1;
        uint32_t ProtocolID : 1;
        uint32_t CTRCode : 5;
        uint32_t NodeGroupID : 4;
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

//CANNet控制帧头
typedef union
{
    struct
    {
        uint8_t DataLen : 4;
        uint8_t RSVD : 1;
        uint8_t UDP : 1;
        uint8_t RTR : 1;
        uint8_t FF : 1;
    }Header;

    uint8_t Value;
}FrameControlHeader;

#pragma pack(1)
//以太网帧
typedef struct EthernetFrame
{
    FrameControlHeader EtherHeader; //帧控制头
    CANFrame_STD CANFrame;
}EtherFrame;

#pragma pack()

uint16_t WordCombine(uint8_t byte1, uint8_t byte2);

#endif // ETHERNETFRAMESHEET_H
