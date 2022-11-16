#include "HALLib.h"
#include <LED.h>
#include <PWM.h>
#include <Power.h>
#include <IOController.h>
#include <IO.h>
#include "Battery.h"


using namespace LM_VSHAL;


HALLib::HALLib(std::string a_GPIOPath, std::string a_PWMPath,
               std::string a_BatteryPath,
               std::string a_BatteryRTinfopath,
               std::string a_BatteryRTcontrolpath,
               std::string a_BatteryRTinfoFunction,
               std::string a_ConfigPath,
               std::string a_PowerLedPath)
{
    m_LED = new LED(a_GPIOPath, a_PowerLedPath);
    m_PWM = new PWM(a_PWMPath);
    m_Power = new Power(a_GPIOPath);
    m_IOController = new IOController(a_GPIOPath);
    m_IO = new IO(a_GPIOPath);
    m_Battery = new Battery(a_BatteryPath, a_BatteryRTinfopath, a_BatteryRTcontrolpath, a_BatteryRTinfoFunction);
    m_ConfigParams = new ConfigParams(a_ConfigPath);
}


HALLib::~HALLib()
{
    delete m_LED;
    delete m_PWM;
    delete m_Power;
    delete m_IOController;
    delete m_IO;
    delete m_ConfigParams;
    delete m_Battery;
}


LED &HALLib::getLED()
{
    return *m_LED;
}


PWM &HALLib::getPWM()
{
    return *m_PWM;
}


Power &HALLib::getPower()
{
    return *m_Power;
}


IOController &HALLib::getIOController()
{
    return *m_IOController;
}


IO &HALLib::getIO()
{
    return *m_IO;
}
