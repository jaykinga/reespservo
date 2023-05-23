#ifndef _THEADCOMMAND_H_
#define _THEADCOMMAND_H_
#include "CommandInfo.h"
#include "Controller.h"
#include "ritos.h"
class ThreadCommand : public Ritos
{
public:
    String ID = "";
    String devicename = "";
    COMMANDINFO commandinfo;
    int delaytime = 0;

public:
    ThreadCommand() {}
    ThreadCommand(const char *_devicename)
    {
        ID = millis();
        devicename = _devicename;
        commandinfo = getCommandInfo(_devicename);
        delaytime = commandinfo.delaytime;
    }
    ~ThreadCommand()
    {
        // Serial.println("我是析构函数TheadONDevice()");
        detach();
    }
};
#endif // !_THREADCOMMANDCLASS_H_#define