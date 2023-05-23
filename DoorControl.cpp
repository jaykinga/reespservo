#include "DoorControl.h"

DoorControl::DoorControl()
{
    this->enableTransmit(D5);
    this->setPulseLength(499);
    // Serial.println("lass DoorControl was successfully initialized");
}
void DoorControl::alllock()
{
    this->send(this->alllockcode,24);  
}
void DoorControl::singleopen()
{
     this->send(this->singleopencode,24);  
}
void DoorControl::clickopen()
{
    this->send(this->normallyopencode,24);
}
void DoorControl::normallyopen(int pressmill = 1500)
{
    
    long begintime = millis();
    while (1)
    {
        delay(50);
        long currenttime = millis();
        int pasttime = currenttime - begintime;
        this->send(this->normallyopencode,24);
        // Serial.printf("pasttime = %d\n", pasttime);
        
        if(pasttime - pressmill >= 0)
        {
            break;
        }
    }
    // Serial.println("time over");
}
void DoorControl::unlock()
{
     this->send(this->unlockcode,24);
}