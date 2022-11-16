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
    //! \param a_batteryInfoPath : The path to battery device information
    //! \param a_batteryRTInfoPath : The path to battery device realtime information
    //! \param a_batteryRTControlPath : The path to battery device control information
    //! \param a_BatteryRTinfoFunction : The function for connection status, i.e. "battery1_present"
    Battery(std::string a_BatteryPath, std::string a_BatteryRTinfopath,
            std::string a_BatteryRTControlpath, std::string a_BatteryRTinfoFunction);
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

    //! \brief Get whether battery is connected based on realtime info
    //! \return bool : true if battery connected
    bool isBatteryConnected();

    //! \brief Set where the status information for the battery can be found (capacity, temp, etc.)
    //! \param a_batteryInfoPath : The path to battery device information
    void setBatteryInfoPath(std::string a_batteryInfoPath);

    //! \brief Set where the realtime status information for the battery can be found (battery connected, charger connected, etc.)
    //! \param a_batteryRTInfoPath : The path to battery device realtime information
    void setBatteryRTInfoPath(std::string a_batteryRTInfoPath);

    //! \brief Set where the realtime control information for the battery can be found (master search, master remove, etc.)
    //! \param a_batteryRTControlPath : The path to battery device control information
    void setBatteryRTControlPath(std::string a_batteryRTControlPath);

    //! \brief Force master search after battery is inserted. If not used, status information for the battery can take up to 90 seconds to be updated
    void forceMasterSearch();

    //! \brief Force master remove after battery is Removed. If not used, status information for the battery can take up to 90 seconds to be updated
    void forceMasterRemove();

private:
    File *m_BatteryFile;
    File *m_BatteryRTInfoFile;
    File *m_BatteryRTControlFile;
    std::string m_rtFunctionName;
};

}
#endif // BATTERY_H
