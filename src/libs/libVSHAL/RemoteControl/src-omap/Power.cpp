#include "Power.h"


using namespace LM_VSHAL;


Power::Power(std::string a_Path)
: m_GPIO(a_Path)
{
}


Power::~Power()
{
}


void Power::setAudioPower(bool a_Enable)
{
    m_GPIO.setGPIO(Types::GPIO_AMP_SDn, a_Enable);
}


bool Power::getAudioPower()
{
    return m_GPIO.getGPIO(Types::GPIO_AMP_SDn);
}


void Power::setWifiPower(bool a_Enable)
{
    m_GPIO.setGPIO(Types::GPIO_PWREN_WIFI, a_Enable);
}


bool Power::getWifiPower()
{
    return m_GPIO.getGPIO(Types::GPIO_PWREN_WIFI);
}


bool Power::getUSBOTGPower()
{
    return m_GPIO.getGPIO(Types::GPIO_OTG_OC);
}
