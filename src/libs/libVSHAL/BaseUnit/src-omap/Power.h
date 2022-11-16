#ifndef POWER_H_
#define POWER_H_


#include <GPIO.h>
#include <HALException.h>
#include <linux/input.h>
#include <fcntl.h>

enum KeyPressStatus_t
{
    KEY_RELEASE = 0,
    KEY_PRESS = 1,
    KEY_KEEPING_PRESSED = 2
};

namespace LM_VSHAL
{

//! \brief Class for Power control.
//! \brief This class is GPIO based.
class Power
{
public:
    //! \brief Constructor
    //! \param a_Path : The path to gpio is sysfs
    Power(std::string a_Path);
    //! \brief Destructor
    virtual ~Power();

    //! \brief Set Manikin power
    //! \param bool : true=on, false=off
    void setManikinPower(bool a_Enable);

    //! \brief Enable Vocal Amplifier
    //! \param bool : true=on, false=off
    void setVocalAmpEnable(bool a_Enable);

    //! \brief get status for Vocal Amplifier power
    //! \return bool : true=on, false=off
    bool getVocalAmpPower();

    //! \brief Get Manikin power
    //! \return bool : true=on, false=off
    bool getManikinPower();

    //! \brief Get power button state
    //! \return bool : true=on, false=off
    bool getPowerButton();
	
    //! \brief Open handle to device
    //! \param a_Device Device to open. Most likely: /dev/input/event0
    //! \return bool : true=success, false=failure
    bool openPowerButtonDevice(const char *a_Device);
 
    //! \brief close file handle to device
    void closePowerButtonDevice();

    //! \brief Set BOOST 12V
    //! \param bool : true=on, false=off
    void setBoost12V(bool a_Enable);

    //! \brief get status for BOOST 12V
    //! \return bool : true=on, false=off
    bool getBoost12V();

private:
    GPIO m_GPIO;
    int	m_FileHandle;
    struct input_event m_InputEvents[64];
    bool m_PreviousPowerButtonValue;
};

}


#endif // POWER_H_
