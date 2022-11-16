#include "IO.h"


using namespace LM_VSHAL;


IO::IO(std::string a_Path)
: m_GPIO(a_Path)
{
}


IO::~IO()
{
}


void IO::setUART4Destination(UARTDestination_t a_Destination)
{
    if( a_Destination == CPR_METER )
    {
        m_GPIO.setGPIO(Types::GPIO_UART4_SEL, true);
    }
    else
    {
        m_GPIO.setGPIO(Types::GPIO_UART4_SEL, false);
    }
}


IO::UARTDestination_t IO::getUART4Destination()
{
    if( m_GPIO.getGPIO(Types::GPIO_MCU_RESET) )
    {
        return BOARD_CONTROLLER;
    }
    else
    {
        return CPR_METER;
    }
}


bool IO::getLineInConnection()
{
    return m_GPIO.getGPIO(Types::GPIO_LINE_IN_DET);
}


bool IO::getLineOutConnection()
{
    return m_GPIO.getGPIO(Types::GPIO_LINE_OUT_DET);
}
