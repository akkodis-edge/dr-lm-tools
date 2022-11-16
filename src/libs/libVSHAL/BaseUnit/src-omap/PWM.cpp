#include <sys/stat.h>
#include <sstream>
#include <stdio.h>

#include <HALException.h>
#include "PWM.h"


using namespace LM_VSHAL;
using namespace std;


pthread_mutex_t global_pwm_mutex = PTHREAD_MUTEX_INITIALIZER;


PWM::PWM(std::string a_Path)
: m_Lock(global_pwm_mutex)
{
    struct stat stFileInfo;

    if( stat(a_Path.c_str(), &stFileInfo) != 0 )
    {
        throw HALException( "PWM: PWM path does not exist: " + a_Path );
    }

    populateMap();
}


PWM::~PWM()
{
}


void PWM::populateMap()
{
    m_PWMMap[PWM_MANIKIN_GPO1]   =  8;
    m_PWMMap[PWM_MANIKIN_GPO2]   =  9;
    m_PWMMap[PWM_MANIKIN_ECG]    = 10;
}


int PWM::findPWM(PWMFunction_t a_Function)
{
    map<PWMFunction_t, int>::const_iterator mapIter;

    mapIter = m_PWMMap.find(a_Function);
    if(mapIter != m_PWMMap.end())
    {
        return mapIter->second;
    }

    return -1;
}


void PWM::setPWM8(int8_t a_DutyCycle)
{
    setPWM(PWM_MANIKIN_GPO1, a_DutyCycle);
}


void PWM::setPWM9(int8_t a_DutyCycle)
{
    setPWM(PWM_MANIKIN_GPO2, a_DutyCycle);
}


void PWM::setECGPWM (int8_t a_DutyCycle)
{
    setPWM(PWM_MANIKIN_ECG, a_DutyCycle);
}


void PWM::setPWM(PWMFunction_t a_Function, int8_t a_Value)
{
    FILE *file = openPWM(a_Function);

    m_Lock.lock();
    fprintf( file, "%d\n", a_Value );
    m_Lock.unlock();

    fclose(file);
}


FILE *PWM::openPWM(PWMFunction_t a_Function)
{
    int pwm = findPWM(a_Function);
    std::ostringstream o;
    o << pwm;
    string path = m_PWMPrefix + o.str();

    FILE *file = fopen(path.c_str(), "r+");
    if( file == NULL )
    {
        throw HALException(path + " does not exist");
    }

    return file;
}
