#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

#include "LCD.h"
#include "HALException.h"


using namespace LM_VSHAL;
using namespace std;


LCD::LCD(std::string a_LcdPath, std::string a_GraphicsPath)
{
    m_PwmFile = new File(a_LcdPath);
    m_GraphicsFile = new File(a_GraphicsPath);
    m_DisplayEnabled = m_GraphicsFile->get("blank") == 0;
}


LCD::~LCD()
{
    delete m_PwmFile;
    delete m_GraphicsFile;
}


int LCD::getMaxBrightness()
{
    return m_PwmFile->get("max_brightness");
}


void LCD::setBrightness(int a_Brightness)
{
    m_PwmFile->set("brightness", a_Brightness);
}


int LCD::getBrightness()
{
    return m_PwmFile->get("brightness");
}

void LCD::setDisplayEnabled(bool a_Enabled)
{
    if(a_Enabled)
    {
        m_GraphicsFile->set("blank", 0);
#ifdef VS_CROSSCOMPILE
        if(!m_DisplayEnabled)
        {
            usleep(600000);
        }
#endif
        m_PwmFile->set("bl_power", 0);
        m_DisplayEnabled = true;
    }
    else
    {
        m_GraphicsFile->set("blank", 1);
        m_PwmFile->set("bl_power", 1);
        m_DisplayEnabled = false;
    }
}

bool LCD::getDisplayEnabled()
{
    return m_DisplayEnabled;
}


