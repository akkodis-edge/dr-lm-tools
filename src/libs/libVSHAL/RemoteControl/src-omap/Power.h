#ifndef POWER_H_
#define POWER_H_


#include <GPIO.h>


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

    //! \brief Set Audio power
    //! \param bool : true=on, false=off
    void setAudioPower(bool a_Enable);

    //! \brief Get Audio power
    //! \return bool : true=on, false=off
    bool getAudioPower();

    //! \brief Set Wifi power
    //! \param bool : true=on, false=off
    void setWifiPower(bool a_Enable);

    //! \brief Get Wifi power
    //! \return bool : true=on, false=off
    bool getWifiPower();

    //! \brief Get USB 'OnTheGo' power
    //! \return bool : true=on, false=off
    bool getUSBOTGPower();

private:
    GPIO m_GPIO;
};

}


#endif // POWER_H_
