#ifndef HALLIB_H_
#define HALLIB_H_


#include <HALLibBase.h>


namespace LM_VSHAL
{

class LED;
class PWM;
class Power;
class IOController;
class IO;
class Battery;

//! \brief Main LM_VSHAL lib class. Contains all other classes. This class is for OMAP.
class HALLib : public HALLibBase
{
public:
    //! \brief Constructor
    //! \note  All parameters has default values
    //! \param a_GPIOPath           : The path to gpio is sysfs
    //! \param a_PWMPath            : The path to the PWM
    //! \param a_BatteryPath        : The path to the battery info
    //! \param a_BatteryRTinfopath  : The path to check that a battery is connected
    //! \param a_BatteryRTcontrolpath: The path to control update of battery info
    //! \param a_ConfigPath         : The path to the eeprom
    HALLib(std::string a_GPIOPath = GPIO_PATH,
           std::string a_PWMPath = PWM_PATH,
           std::string a_BatteryPath = BATTERY_DEV_PATH_BU,
           std::string a_BatteryRTinfopath = BATTERY_RT_INFO_PATH_BU,
           std::string a_BatteryRTcontrolpath = "",
           std::string a_BatteryRTinfoFunction = "",
           std::string a_ConfigPath = CONFIG_PATH_BU,
           std::string a_PowerLedPath = LED_CONTROL_PATH_BU);
    //! \brief Destructor
    virtual ~HALLib();

    //! \brief Get LED class
    //! \return The LED class
    LED &getLED();

    //! \brief Get PWM class
    //! \return The PWM class
    PWM &getPWM();

    //! \brief Get Power class
    //! \return The Power class
    Power &getPower();

    //! \brief Get IOController class
    //! \return The IOController class
    IOController &getIOController();

    //! \brief Get IO class
    //! \return The IO class
    IO &getIO();

private:
    LED *m_LED;
    PWM *m_PWM;
    Power *m_Power;
    IOController *m_IOController;
    IO *m_IO;
    Battery *m_Battery;
};

}


#endif // HALLIB_H_
