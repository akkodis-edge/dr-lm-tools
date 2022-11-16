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
    m_GPIOMap[Types::GPIO_LCDPWR_EN]	=  55;
    m_GPIOMap[Types::GPIO_LEDPWR_EN]    =  58;
    m_GPIOMap[Types::GPIO_BST5V_PWREN]  =  98;
    m_GPIOMap[Types::GPIO_V3V_PWREN]    =  96;
    m_GPIOMap[Types::GPIO_PHY_PWREN]    = 111;
    m_GPIOMap[Types::GPIO_PHY_RESET]    =  97;
    m_GPIOMap[Types::GPIO_LAN_PWREN]    =  95;
    m_GPIOMap[Types::GPIO_RESET_LAN]    = 167;
    m_GPIOMap[Types::GPIO_PWREN_TH]     = 142;
    m_GPIOMap[Types::GPIO_AMP_SDn]      = 158;
    m_GPIOMap[Types::GPIO_PWREN_WIFI]   = 159;
    m_GPIOMap[Types::GPIO_OTG5V_EN]     =  26;
    m_GPIOMap[Types::GPIO_OTG_OC]       =  27;
    m_GPIOMap[Types::GPIO_RESET_WLANn]  = 170;
}
