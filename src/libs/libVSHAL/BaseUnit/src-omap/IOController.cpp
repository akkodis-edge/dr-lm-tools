#include "IOController.h"


using namespace LM_VSHAL;


IOController::IOController(std::string a_Path)
: m_GPIO(a_Path)
{
}


IOController::~IOController()
{
}


void IOController::setBootMode(IOCtrlBootMode_t a_BootMode)
{
    switch( a_BootMode )
    {
    case IOCTRLBOOT_NORMAL:
        m_GPIO.setGPIO(Types::GPIO_MCU_BOOT_0, false);
        m_GPIO.setGPIO(Types::GPIO_MCU_BOOT_1, false);
        break;
    case IOCTRLBOOT_REPROGRAM:
        m_GPIO.setGPIO(Types::GPIO_MCU_BOOT_0, false);
        m_GPIO.setGPIO(Types::GPIO_MCU_BOOT_1, true);
        break;
    }
}


IOController::IOCtrlBootMode_t IOController::getBootMode()
{
    bool boot_0 = m_GPIO.getGPIO(Types::GPIO_MCU_BOOT_0);
    bool boot_1 = m_GPIO.getGPIO(Types::GPIO_MCU_BOOT_1);

    if( !boot_0 && !boot_1)
    {
        return IOCTRLBOOT_NORMAL;
    }
    else
    {
        return IOCTRLBOOT_REPROGRAM;
    }
}


void IOController::reset(bool a_Reset)
{
    m_GPIO.setGPIO(Types::GPIO_MCU_RESET, !a_Reset);
}


bool IOController::reset()
{
    return !m_GPIO.getGPIO(Types::GPIO_MCU_RESET);
}
