#ifndef HALLIB_H_
#define HALLIB_H_


#include "HALLibBase.h"


namespace LM_VSHAL
{

class Power;
class LCD;
class Battery;
class LED;

//! \brief Main LM_VSHAL lib class. Contains all other classes. This class is for OMAP.
class HALLib : public HALLibBase
{
public:
    //! \brief Constructor
    //! \note  All parameters has default values
    //! \param a_GPIOPath       : The path to gpio is sysfs
    //! \param a_LCDPath        : The path to the PWM
    //! \param a_GraphicsPath   : The path to Graphics Device needed for display off/on
    //! \param a_BatteryPath    : The path to battery
    //! \param a_ConfigPath     : The path to the eeprom
    HALLib(std::string a_GPIOPath = GPIO_PATH,
           std::string a_LCDPath = LCD_PATH,
           std::string a_GraphicsPath = GRAPHICS_DEV_PATH,
           std::string a_BatteryPath = BATTERY_DEV_PATH_RC,
           std::string a_ConfigPath = CONFIG_PATH_RC,
           std::string a_LEDpath = LED_CONTROL_PATH_RC);
    //! \brief Destructor
    virtual ~HALLib();

    //! \brief Get Power class
    //! \return The Power class
    Power &getPower();

    //! \brief Get LCD class
    //! \return The LCD class
    LCD &getLCD();

    //! \brief Get Battery class
    //! \return The Battery class
    Battery &getBattery();

    //! \brief Get LED class
    //! \return The LED class
    LED &getLED();

private:
    Power *m_Power;
    LCD *m_LCD;
    Battery *m_Battery;
    LED *m_LED;
};

}


#endif // HALLIB_H_
