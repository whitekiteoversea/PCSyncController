#ifndef CCCCONTROL_H
#define CCCCONTROL_H

typedef struct {
    double flat[3];
    double feedbackCoeff[3]; // 交叉反馈系数
}SYNCON;

#endif // CCCCONTROL_H
