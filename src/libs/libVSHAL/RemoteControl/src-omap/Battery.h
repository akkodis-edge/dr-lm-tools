#ifndef BATTERY_H
#define BATTERY_H

#include <string>
#include "File.h"

namespace LM_VSHAL
{

//! \brief Class for battery based control.
//! \brief This class is PWM based.
class Battery
{
public:
    enum  batteryStatus_t
    {
        bsCharing_t,
        bsNotCharing_t,
        bsDischarging_t
    };
    //! \brief Constructor
    //! \param a_Path : The path to battery device
    Battery(std::string a_Path);
    //! \brief Destructor
    virtual ~Battery();

    //! \brief Get Charger connect
    //! \return bool : true if charger is connected otherwise false
    bool isChargerConnected();

    //! \brief Get battery temp
    //! \return int : Temperature of the battery
    int temp();

    //! \brief Get battery status
    //! \return batteryStatus_t : Current status of the battery
    batteryStatus_t status();

    //! \brief Get battery capacity
    //! \return int : Capacity of the battery
    int capacity();

    //! \brief Get Time left on battery
    //! \note Might not be available
    //! \return int : Time until battery is empty in minuttes
    int timeToEmpty();

private:
    File *m_BatteryFile;
};

}
#endif // BATTERY_H
