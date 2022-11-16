#include <sstream>
#include <stdlib.h>

#include "HALLib.h"
#include <Power.h>
#include <LCD.h>
#include <ConfigParams.h>
#include <HALException.h>


using namespace LM_VSHAL;
using namespace std;


HALLib::HALLib(std::string a_GPIOPath, std::string a_LCDPath, std::string a_ConfigPath)
{
    // Populate simulator paths
    string command = "mkdir -p " + a_GPIOPath;
    if( system( command.c_str() ) == -1 )
    {
        throw HALException( "HALLib: Could not create path: " + a_GPIOPath );
    }

    // GPIO
    GPIO gpio(a_GPIOPath);
    std::map<Types::GPIOFunction_t, int> gpioMap = gpio.getGPIOMap();
    map<Types::GPIOFunction_t, int>::const_iterator it;
    for ( it=gpioMap.begin(); it != gpioMap.end(); it++ )
    {
        ostringstream o;
        o << it->second;
        command = "mkdir -p " + a_GPIOPath + "/gpio" + o.str();
        if( system( command.c_str() ) == -1 )
        {
            throw HALException( "HALLib: Could not create path: " + a_GPIOPath + "/gpio" + o.str() );
        }

        command = "echo 0 > " + a_GPIOPath + "/gpio" + o.str() + "/value";
        if( system( command.c_str() ) == -1 )
        {
            throw HALException( "HALLib: Could not create file: " + a_GPIOPath + "/gpio" + o.str() + "/value" );
        }
    }
    m_Power = new Power(a_GPIOPath);

    // PWM
    command = "mkdir -p " + a_LCDPath;
    if( system( command.c_str() ) == -1 )
    {
        throw HALException( "HALLib: Could not create path: " + a_LCDPath );
    }
    m_LCD = new LCD(a_LCDPath);

    // Config
    m_ConfigParams = new ConfigParams(a_ConfigPath);
}


HALLib::~HALLib()
{
    delete m_Power;
    delete m_LCD;
    delete m_ConfigParams;
}


Power &HALLib::getPower()
{
    return *m_Power;
}


LCD &HALLib::getLCD()
{
    return *m_LCD;
}
