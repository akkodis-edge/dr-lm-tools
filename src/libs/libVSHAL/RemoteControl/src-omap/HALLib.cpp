#include "HALLib.h"
#include "Power.h"
#include "LCD.h"
#include "Battery.h"
#include "LED.h"

using namespace LM_VSHAL;


HALLib::HALLib(std::string a_GPIOPath,
               std::string a_LCDPath,
               std::string a_GraphicsPath,
               std::string a_BatteryPath,
               std::string a_ConfigPath,
               std::string a_LEDpath)
{
    m_Power = new Power(a_GPIOPath);
    m_LCD = new LCD(a_LCDPath, a_GraphicsPath);
    m_Battery = new Battery(a_BatteryPath);
    m_ConfigParams = new ConfigParams(a_ConfigPath);
    m_LED = new LED(a_LEDpath);
}


HALLib::~HALLib()
{
    delete m_Power;
    delete m_LCD;
    delete m_Battery;
    delete m_ConfigParams;
    delete m_LED;
}


Power &HALLib::getPower()
{
    return *m_Power;
}


LCD &HALLib::getLCD()
{
    return *m_LCD;
}

Battery & HALLib::getBattery()
{
    return *m_Battery;
}

LED & HALLib::getLED()
{
    return *m_LED;
}
