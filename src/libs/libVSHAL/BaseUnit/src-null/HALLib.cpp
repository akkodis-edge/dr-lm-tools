#include <sstream>
#include <stdlib.h>

#include "HALLib.h"
#include <LED.h>
#include <PWM.h>
#include <Power.h>
#include <IOController.h>
#include <IO.h>
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
    m_LED = new LED(a_GPIOPath);
    m_Power = new Power(a_GPIOPath);
    m_IOController = new IOController(a_GPIOPath);
    m_IO = new IO(a_GPIOPath);

    // PWM
    command = "mkdir -p " + a_LCDPath;
    if( system( command.c_str() ) == -1 )
    {
        throw HALException( "HALLib: Could not create path: " + a_LCDPath );
    }
    m_PWM = new PWM(a_LCDPath);

    // Config
    command = "mkdir -p " + a_ConfigPath;
    if( system( command.c_str() ) == -1 )
    {
        throw HALException( "HALLib: Could not create path: " + a_LCDPath );
    }
    m_ConfigParams = new ConfigParams(a_ConfigPath);
}


HALLib::~HALLib()
{
    delete m_LED;
    delete m_PWM;
    delete m_Power;
    delete m_IOController;
    delete m_IO;
}


LED &HALLib::getLED()
{
    return *m_LED;
}


PWM &HALLib::getPWM()
{
    return *m_PWM;
}


Power &HALLib::getPower()
{
    return *m_Power;
}


IOController &HALLib::getIOController()
{
    return *m_IOController;
}


IO &HALLib::getIO()
{
    return *m_IO;
}
