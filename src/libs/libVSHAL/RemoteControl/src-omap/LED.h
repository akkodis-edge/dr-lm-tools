#ifndef LED_H
#define LED_H

#include <string>
#include "File.h"

namespace LM_VSHAL
{

//! \brief Class for LED control.
//! \brief This class is PWM based.
class LED
{
public:

    //! \brief Enum describing LED modes
    typedef enum
    {
        LED_OFF,
        LED_ON,
        LED_BLINK,
    } LEDmode_t;

    //! \brief Enum describing LED
    typedef enum
    {
        LED_RED,
        LED_GREEN,
        LED_BLUE
    } LED_t;

    //! \brief Constructor
    //! \param a_LEDcontrolPath : The path to control LEDs
    LED(std::string a_LEDcontrolPath);

    //! \brief Destructor
    virtual ~LED();

    //! \brief Set LED
    //! \param a_LED : The LED to set
    //! \param a_LEDmode : LED mode
    void setLED(LED_t a_LED, LEDmode_t a_LEDmode);


private:
    File *m_LEDFile;

};

}
#endif // LED_H
