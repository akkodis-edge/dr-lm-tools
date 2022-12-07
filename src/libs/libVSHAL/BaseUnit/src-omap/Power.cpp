#include "Power.h"
#include <unistd.h>

using namespace LM_VSHAL;


Power::Power(std::string a_Path)
: m_GPIO(a_Path), m_PreviousPowerButtonValue(false)
//, m_iocGPIO_5VManikinEn ()
//, m_iocGPIO_Boost12VEn
//, m_iocGPIO_vocalAmpEn
{
    m_FileHandle = NULL;
}


Power::~Power()
{
    closePowerButtonDevice();
}

/*void Power::setVocalAmpEnable(bool a_Enable)
{
    m_GPIO.setGPIO(Types::GPIO_VOCAL_AMP_EN, a_Enable);
}

bool Power::getVocalAmpPower()
{
    return m_GPIO.getGPIO(Types::GPIO_VOCAL_AMP_EN);
}*/

void Power::setManikinPower(bool a_Enable)
{
    //m_GPIO.setGPIO(Types::GPIO_5V_MANIKIN_EN, a_Enable);
    if ( a_Enable )    
        system("echo 1 >/sys/class/power_supply/MANIKIN_5V/online");
    else
        system("echo 0 >/sys/class/power_supply/MANIKIN_5V/online");
}


bool Power::getManikinPower()
{
    return m_GPIO.getGPIO(Types::GPIO_5V_MANIKIN_EN);
}

void Power::closePowerButtonDevice()
{
    if (NULL != m_FileHandle)
    {
        close(m_FileHandle);
    }
}

bool Power::openPowerButtonDevice(const char *a_Device)
{
    m_FileHandle = open(a_Device, O_RDONLY | O_NONBLOCK);
    if (0 > m_FileHandle)
    {
        return false;
    }
    return true;
}

bool Power::getPowerButton()
{
    if (NULL == m_FileHandle)
    {
        throw new HALException("Power::getPowerButton(): File handle to Power Button device not open");
    }
    ssize_t bytesRead = read(m_FileHandle, m_InputEvents, sizeof(m_InputEvents));

    if (0 >= bytesRead ||bytesRead < (int) sizeof(struct input_event))
    {
        return m_PreviousPowerButtonValue;
    }

    for (int Ctr = 0; Ctr < (int) (bytesRead / sizeof(struct input_event)); ++Ctr)
    {
        if (EV_KEY == m_InputEvents[Ctr].type)
        {
            if (KEY_PRESS == m_InputEvents[Ctr].value)
            {
		m_PreviousPowerButtonValue=true;
            }
            else if (KEY_RELEASE == m_InputEvents[Ctr].value)
            {
		m_PreviousPowerButtonValue=false;
            }
        }
    }
    return m_PreviousPowerButtonValue;
}

void Power::setBoost12V(bool a_Enable)
{
    //m_GPIO.setGPIO(Types::GPIO_BOOST_12V_EN, a_Enable);
    if(a_Enable)
        system("echo 1 >/sys/class/power_supply/MANIKIN_5V/online");
    else
        system("echo 0 >/sys/class/power_supply/MANIKIN_5V/online");
}

bool Power::getBoost12V()
{
    return m_GPIO.getGPIO(Types::GPIO_BOOST_12V_EN);
}
