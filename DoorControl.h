#ifndef _DOORCONTROL_H_
#define _DOORCONTROL_H_
#include <RCSwitch.h>
class DoorControl : public RCSwitch
{
private:
    int alllockcode = 356544;      // A 全锁代码
    int singleopencode = 356400;   // B 单向代码
    int normallyopencode = 356364; // C 开代码
    int unlockcode = 356355;       // D 开锁代码

public:
    DoorControl(); // 构造函数

    // 遥控器功能接口
    void alllock();      // 全锁
    void singleopen();   // 单向
    void normallyopen(int pressmill); // 常开
    void clickopen();    // 点开
    void unlock();       // 解锁
};

#endif //_DOORCONTROL_H_