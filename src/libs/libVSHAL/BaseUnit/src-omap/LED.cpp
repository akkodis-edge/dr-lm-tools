#include <semaphore.h>
#include <iostream>
#include <sstream>

#include "LED.h"


using namespace LM_VSHAL;
using namespace std;


LED::LED(std::string a_WIFILEDPath, std::string a_PowerLEDpath)
: m_GPIO(a_WIFILEDPath)
{
    m_LEDFile = new File(a_PowerLEDpath);
    populateMap();
}


LED::~LED()
{
    delete m_LEDFile;
}


void LED::populateMap()
{
    m_PowerLedMap[LED_POWER_BLACK]      = "black";
    m_PowerLedMap[LED_POWER_BLUE]       = "blue";
    m_PowerLedMap[LED_POWER_GREEN]      = "green";
    m_PowerLedMap[LED_POWER_CYAN]       = "cyan";
    m_PowerLedMap[LED_POWER_RED]        = "red";
    m_PowerLedMap[LED_POWER_MAGENTA]    = "magenta";
    m_PowerLedMap[LED_POWER_YELLOW]     = "yellow";
    m_PowerLedMap[LED_POWER_WHITE]      = "white";

    m_WIFIledMap[LED_WIFI_RED]          = Types::GPIO_WIFI_LED_R;
    m_WIFIledMap[LED_WIFI_GREEN]        = Types::GPIO_WIFI_LED_G;
    m_WIFIledMap[LED_WIFI_BLUE]         = Types::GPIO_WIFI_LED_B;
}


std::string LED::findPowerLedColor(PowerLedColor_t a_LEDColor)
{
    map<PowerLedColor_t, std::string>::const_iterator mapIter;

    mapIter = m_PowerLedMap.find(a_LEDColor);
    if(mapIter != m_PowerLedMap.end())
    {
        return mapIter->second;
    }

    return "";
}

Types::GPIOFunction_t LED::findWifiLedFunction(WifiLedColor_t a_LEDColor)
{
    map<WifiLedColor_t, Types::GPIOFunction_t>::const_iterator mapIter;

    mapIter = m_WIFIledMap.find(a_LEDColor);
    if(mapIter != m_WIFIledMap.end())
    {
        return mapIter->second;
    }

    return Types::GPIO_UNUSED;
}

void LED::setGpioWifiLed(WifiLedColor_t a_LEDColor, bool a_State)
{
    m_GPIO.setGPIO(findWifiLedFunction(a_LEDColor), a_State);
}



bool LED::getWifiLED(WifiLedColor_t a_LEDColor)
{
    return m_GPIO.getGPIO(findWifiLedFunction(a_LEDColor));
}


void LED::setPowerLED(PowerLedCtrl_t a_mode)
{
    std::string color;
    std::string blink = "yes";
    std::string blinkOn;
    std::string blinkOff;

    stringstream ssBlinkOn;
    stringstream ssBlinkOff;
    ssBlinkOn << a_mode.m_BlinkRateOn;
    ssBlinkOff << a_mode.m_BlinkRateOff;

    blinkOn = ssBlinkOn.str();
    blinkOff = ssBlinkOff.str();

    if(a_mode.m_Blink == LED_BLINK_OFF)
    {
        blink = "no";
    }

    color = findPowerLedColor((PowerLedColor_t)a_mode.m_Color);
    if(color.length())
    {
        m_LEDFile->setString("led", color);
        m_LEDFile->setString("blink", blink);
        m_LEDFile->setString("blink_off_time", blinkOff);
        m_LEDFile->setString("blink_on_time", blinkOn);
    }

}


void LED::setWifiLED(WIFILEDRGB_t a_ColorMode)
{
    switch(a_ColorMode)
    {
    case LED_ALL_OFF:
        setGpioWifiLed(LED_WIFI_RED, false);
        setGpioWifiLed(LED_WIFI_GREEN, false);
        setGpioWifiLed(LED_WIFI_BLUE, false);
        break;
    case LED_RED_ON:
        setGpioWifiLed(LED_WIFI_RED, true);
        setGpioWifiLed(LED_WIFI_BLUE, false);
        setGpioWifiLed(LED_WIFI_GREEN, false);
        break;
    case LED_RED_GREEN_ON:
        setGpioWifiLed(LED_WIFI_RED, true);
        setGpioWifiLed(LED_WIFI_GREEN, true);
        setGpioWifiLed(LED_WIFI_BLUE, false);
        break;
    case LED_RED_GREEN_BLUE_ON:
        setGpioWifiLed(LED_WIFI_RED, true);
        setGpioWifiLed(LED_WIFI_GREEN, true);
        setGpioWifiLed(LED_WIFI_BLUE, true);
        break;
    case LED_RED_BLUE_ON:
        setGpioWifiLed(LED_WIFI_RED, true);
        setGpioWifiLed(LED_WIFI_BLUE, true);
        setGpioWifiLed(LED_WIFI_GREEN, false);
        break;
    case LED_GREEN_ON:
        setGpioWifiLed(LED_WIFI_GREEN, true);
        setGpioWifiLed(LED_WIFI_BLUE, false);
        setGpioWifiLed(LED_WIFI_RED, false);
        break;
    case LED_GREEN_BLUE_ON:
        setGpioWifiLed(LED_WIFI_GREEN, true);
        setGpioWifiLed(LED_WIFI_BLUE, true);
        setGpioWifiLed(LED_WIFI_RED, false);
        break;
    case LED_BLUE_ON:
        setGpioWifiLed(LED_WIFI_BLUE, true);
        setGpioWifiLed(LED_WIFI_GREEN, false);
        setGpioWifiLed(LED_WIFI_RED, false);
        break;
   }
}

