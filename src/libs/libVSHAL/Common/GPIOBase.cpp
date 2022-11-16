#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <stdio.h>

#include <HALException.h>
#include "GPIOBase.h"


using namespace LM_VSHAL;
using namespace std;


pthread_mutex_t global_gpio_mutex = PTHREAD_MUTEX_INITIALIZER;


GPIOBase::GPIOBase(std::string a_Path)
: m_Lock(global_gpio_mutex),
  m_GPIOPrefix(a_Path + "/" + "gpio")
{
    struct stat stFileInfo;

    if( stat(a_Path.c_str(), &stFileInfo) != 0 )
    {
        throw HALException( "GPIO: GPIO path does not exist: " + a_Path );
    }
}


GPIOBase::~GPIOBase()
{
}


int GPIOBase::findPin(Types::GPIOFunction_t a_GPIOFunction)
{
    map<Types::GPIOFunction_t, int>::const_iterator mapIter;

    mapIter = m_GPIOMap.find(a_GPIOFunction);
    if(mapIter != m_GPIOMap.end())
    {
        return mapIter->second;
    }

    return -1;
}


void GPIOBase::setGPIO(Types::GPIOFunction_t a_GPIOFunction, bool a_State)
{
    FILE *file = openGPIO(a_GPIOFunction);

    m_Lock.lock();
    fprintf( file, "%d\n", a_State ? 1 : 0 );
    m_Lock.unlock();

    fclose(file);
}


bool GPIOBase::getGPIO(Types::GPIOFunction_t a_GPIOFunction)
{
    FILE *file = openGPIO(a_GPIOFunction);
    if (file == NULL)
        return false;

    int data;

    m_Lock.lock();
    fscanf( file, "%d\n", &data );
    m_Lock.unlock();

    fclose(file);

    return (bool)data;
}


FILE *GPIOBase::openGPIO(Types::GPIOFunction_t a_GPIOFunction)
{
    int pin = findPin(a_GPIOFunction);
    std::ostringstream o;
    o << pin;
    string path = m_GPIOPrefix + o.str() + "/value";

    FILE *file = fopen(path.c_str(), "r+");
    if( file == NULL )
    {
        cout << path.c_str() << " does not exist" << endl;
        perror ("The following error occurred");
        throw HALException(path + " does not exist");



    }

    return file;
}


std::map<Types::GPIOFunction_t, int> GPIOBase::getGPIOMap()
{
    return m_GPIOMap;
}
