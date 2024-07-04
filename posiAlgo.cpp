#include "posiAlgo.h"
#include <cmath>

#define PI (3.1415927)

/*
输入：理论质心给定位置参考和实际计算质心的差值 - um
     当前控制周期，用于产生离散积分和离散微分 - ms

输出：Spid

*/
int pid_ftSMC_cal(int wh_Offset, unsigned short discretization_Ms)
{
    int output_Spid = 0;
    // PID系数
    int kp = 0;
    int ki = 0;
    int kd = 0;

    // 滑模面系数
    int k1 = 0;
    int k2 = 0;
    float sigma = 0.5; // 0-1

    static int sumoffset = 0;
    static int sSurface = 0;
    static int sum_sSurface = 0;
    static int latest_whOffset = 0; // 上一周期记录的质心误差
    static int latest_sSurface = 0;

    sumoffset += wh_Offset;

    // 离散滑模面更新
    if (discretization_Ms > 0) {
        sSurface = (wh_Offset - latest_whOffset)/discretization_Ms + k1 * wh_Offset + k2 * pow(abs(wh_Offset), (sigma-1))*2/PI*atan(wh_Offset);

        sum_sSurface += sSurface;
        output_Spid = kp*sSurface + ki*sum_sSurface +kd * (sSurface - latest_sSurface)/discretization_Ms;


        latest_sSurface = sSurface;
    }
    latest_whOffset  = wh_Offset;
    // 限幅处理（暂无）

    return output_Spid;
}
