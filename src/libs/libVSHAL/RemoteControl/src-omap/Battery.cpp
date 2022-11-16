#include "Battery.h"

using namespace LM_VSHAL;

Battery::Battery(std::string a_Path)
{
    m_BatteryFile = new File(a_Path);
}

Battery::~Battery()
{
    delete m_BatteryFile;
}

bool Battery::isChargerConnected()
{
#ifdef VS_SYSTEMCONTROLLER_RC
    return (bool) !m_BatteryFile->get("power_now");
#else
    return (bool) m_BatteryFile->get("online");
#endif
}

int Battery::temp()
{
#ifdef VS_SYSTEMCONTROLLER_RC
    return 35;
#else
    return m_BatteryFile->get("temp");
#endif
}

Battery::batteryStatus_t Battery::status()
{
    Battery::batteryStatus_t result = bsDischarging_t;
    std::string status = m_BatteryFile->getString("status");
#ifdef VS_SYSTEMCONTROLLER_RC
    if(status == "Unknown")
    {
        result = bsCharing_t;
    }
#else
    if(status == "Charging")
    {
        result = bsCharing_t;
    }
    else if(status =="Not charging")
    {
        result = bsNotCharing_t;
    }
#endif
    return result;
}

int Battery::capacity()
{
#ifdef VS_SYSTEMCONTROLLER_RC
    double full = m_BatteryFile->get("energy_full");
    double now = m_BatteryFile->get("energy_now");
    return (now / full) * 100;
#else
    return m_BatteryFile->get("capacity");
#endif
}

int LM_VSHAL::Battery::timeToEmpty()
{
#ifdef VS_SYSTEMCONTROLLER_RC
    return 100;
#else
    return m_BatteryFile->get("time_to_empty_avg");
#endif
}

