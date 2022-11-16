#ifndef LCD_H_
#define LCD_H_

#include <string>
#include "File.h"

namespace LM_VSHAL
{

//! \brief Class for LCD based control.
//! \brief This class is PWM based.
class LCD
{
public:
    //! \brief Constructor
    //! \param a_Path : The path to PWM device
    LCD(std::string a_PwmPath, std::string a_GraphicsPath);
    //! \brief Destructor
    virtual ~LCD();

    //! \brief Get max brightness
    //! \return int : Max brightness
    int getMaxBrightness();

    //! \brief Set LCD brightness
    //! \param int : LCD brightness
    void setBrightness(int a_Brightness);

    //! \brief Get LCD brightness
    //! \return int : LCD brightness
    int getBrightness();

    //! \brief Enables display
    //! \param bool : true will enabled the display, false disable display
    void setDisplayEnabled(bool a_Enabled);

    //! \brief Enables display
    //! \return bool : true will enabled the display, false disable display
    bool getDisplayEnabled();

private:
    File *m_PwmFile;
    File *m_GraphicsFile;
    bool m_DisplayEnabled;
};

}


#endif // LCD_H_
