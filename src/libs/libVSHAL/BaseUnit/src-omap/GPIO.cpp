#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <stdio.h>

#include <HALException.h>
#include "GPIO.h"


using namespace LM_VSHAL;
using namespace std;


GPIO::GPIO(std::string a_Path)
: GPIOBase(a_Path)
{
    populateMap();
}


GPIO::~GPIO()
{
}


void GPIO::populateMap()
{
    m_GPIOMap[Types::GPIO_BOOST_12V_EN]         = 36;
    m_GPIOMap[Types::GPIO_MCU_BOOT_1]           = 66;
    m_GPIOMap[Types::GPIO_MCU_RESET]            = 67;
    m_GPIOMap[Types::GPIO_UART4_SEL]            = 68;
    m_GPIOMap[Types::GPIO_VOCAL_AMP_EN]		= 84;
    m_GPIOMap[Types::GPIO_5V_MANIKIN_EN] 	= 89;
    m_GPIOMap[Types::GPIO_LINE_IN_DET]          = 90;
    m_GPIOMap[Types::GPIO_LINE_OUT_DET]         = 91;
    m_GPIOMap[Types::GPIO_PWRBT]                = 92;
    m_GPIOMap[Types::GPIO_MCU_BOOT_0]           = 93;
    m_GPIOMap[Types::GPIO_PWR_LED_R]            = 94;
    m_GPIOMap[Types::GPIO_PWR_LED_B]            = 178;
    m_GPIOMap[Types::GPIO_PWR_LED_G]            = 179;
    m_GPIOMap[Types::GPIO_WIFI_LED_B]           = 180;
    m_GPIOMap[Types::GPIO_WIFI_LED_G]           = 181;
    m_GPIOMap[Types::GPIO_WIFI_LED_R]           = 182;
}
