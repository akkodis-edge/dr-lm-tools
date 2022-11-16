#include "Battery.h"

using namespace LM_VSHAL;

Battery::Battery(std::string a_BatteryPath, std::string a_BatteryRTinfopath,
    std::string a_BatteryRTControlpath, std::string a_BatteryRTinfoFunction)
{
    m_BatteryFile = NULL;
    m_BatteryRTInfoFile = NULL;
    m_BatteryRTControlFile = NULL;

    if(a_BatteryPath.length())
    {
        m_BatteryFile = new File(a_BatteryPath);
    }

    if(a_BatteryRTinfopath.length())
    {
        m_BatteryRTInfoFile = new File(a_BatteryRTinfopath);
    }

    if(a_BatteryRTControlpath.length())
    {
        m_BatteryRTControlFile = new File(a_BatteryRTControlpath);
    }

    m_rtFunctionName = a_BatteryRTinfoFunction;

}

Battery::~Battery()
{
    delete m_BatteryFile;
    delete m_BatteryRTInfoFile;
}

bool Battery::isChargerConnected()
{
    if(m_BatteryRTInfoFile)
    {
        std::string status = m_BatteryRTInfoFile->getString("adapter_present");
        if(status == "yes")
        {
            return true;
        }
    }

    return false;
}

int Battery::temp()
{
    if(m_BatteryFile)
    {
        return m_BatteryFile->get("temp");
    }

    return 0;
}

Battery::batteryStatus_t Battery::status()
{
    Battery::batteryStatus_t result = bsDischarging_t;

    if(m_BatteryFile)
    {
        std::string status = m_BatteryFile->getString("status");
        if(status == "Charging")
        {
            result = bsCharing_t;
        }
        else if(status =="Not charging")
        {
            result = bsNotCharing_t;
        }
    }

    return result;
}

int Battery::capacity()
{
    if(m_BatteryFile)
    {
        return m_BatteryFile->get("capacity");
    }

    return 0;
}

int LM_VSHAL::Battery::timeToEmpty()
{
    if(m_BatteryFile)
    {
        return m_BatteryFile->get("time_to_empty_now");
    }

    return 0;
}

bool Battery::isBatteryConnected()
{
    if(m_BatteryRTInfoFile)
    {
        std::string status = m_BatteryRTInfoFile->getString(m_rtFunctionName);
        if(status == "yes")
        {
            return true;
        }
    }

    return false;

}

void Battery::setBatteryInfoPath(std::string a_batteryInfoPath)
{
    if(m_BatteryFile)
    {
        delete m_BatteryFile;
        m_BatteryFile = NULL;
    }

    if(a_batteryInfoPath.length())
    {
        m_BatteryFile = new File(a_batteryInfoPath);
    }
}

void Battery::setBatteryRTInfoPath(std::string a_batteryRTInfoPath)
{
    if(m_BatteryRTInfoFile)
    {
        delete m_BatteryRTInfoFile;
        m_BatteryRTInfoFile = NULL;
    }

    if(a_batteryRTInfoPath.length())
    {
        m_BatteryRTInfoFile = new File(a_batteryRTInfoPath);
    }
}

void Battery::setBatteryRTControlPath(std::string a_batteryRTControlPath)
{
    if(m_BatteryRTControlFile)
    {
        delete m_BatteryRTControlFile;
        m_BatteryRTControlFile = NULL;
    }

    if(a_batteryRTControlPath.length())
    {
        m_BatteryRTControlFile = new File(a_batteryRTControlPath);
    }
}

void Battery::forceMasterSearch()
{
    if(m_BatteryRTControlFile)
    {
        m_BatteryRTControlFile->set("w1_master_search", 1);
    }
}

void Battery::forceMasterRemove()
{
    if(m_BatteryRTControlFile)
    {
        std::string devId = m_BatteryRTControlFile->getString("w1_master_slaves");
        m_BatteryRTControlFile->setString("w1_master_remove", devId);
    }
}
