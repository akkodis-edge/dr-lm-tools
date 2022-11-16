#include "LED.h"

using namespace LM_VSHAL;

LED::LED(std::string a_LEDcontrolPath)
{
    m_LEDFile = new File(a_LEDcontrolPath);
}

LED::~LED()
{
    delete m_LEDFile;
}

void LED::setLED(LED_t a_LED, LEDmode_t a_LEDmode)
{
    std::string func;
    std::string mode;

    switch(a_LED)
    {
        case LED_RED:
            func = "led_red";
            break;
        case LED_GREEN:
            func = "led_green";
            break;
        case LED_BLUE:
            func = "led_blue";
            break;
    }

    switch(a_LEDmode)
    {
        case LED_OFF:
            mode = "off";
            break;
        case LED_ON:
            mode = "on";
            break;
        case LED_BLINK:
            mode = "blink";
            break;
    }

    if(func.length() && mode.length())
    {
        m_LEDFile->setString(func, mode);
    }
}
