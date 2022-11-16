#ifndef LED_H_
#define LED_H_


#include <GPIO.h>
#include <stdint.h>
#include <map>
#include <string>
#include "File.h"


namespace LM_VSHAL
{


//! \brief Class for the LEDs.
//! \brief This class is GPIO based.

class LED
{
public:
    //! \brief Constructor
    //! \param a_Path : The path to gpio is sysfs
    LED(std::string a_WIFILEDPath, std::string a_PowerLEDpath);
    //! \brief Destructor
    virtual ~LED();

    //! \brief Enum describing PowerLEDs and colors
    typedef enum
    {
        LED_POWER_BLACK,
        LED_POWER_BLUE,
        LED_POWER_GREEN,
        LED_POWER_CYAN,
        LED_POWER_RED,
        LED_POWER_MAGENTA,
        LED_POWER_YELLOW,
        LED_POWER_WHITE
    } PowerLedColor_t;

    //! \brief Enum power LED blink rates
    typedef enum
    {
        LED_BLINK_OFF = 0,
        LED_BLINK_250MS = 25,
        LED_BLINK_500MS = 50,
        LED_BLINK_1000MS = 100
    } LEDBlink_t;

    struct PowerLedCtrl_t
    {
        PowerLedCtrl_t() : m_Color(LED_POWER_BLACK), m_Blink(false), m_BlinkRateOn(LED_BLINK_OFF), m_BlinkRateOff(LED_BLINK_OFF)
        {
        }

        PowerLedCtrl_t(PowerLedColor_t color, bool blink, LEDBlink_t blinkRateOn, LEDBlink_t blinkRateOff) :
            m_Color(color), m_Blink(blink), m_BlinkRateOn(blinkRateOn), m_BlinkRateOff(blinkRateOff)
        {
        }

        PowerLedColor_t m_Color;
        bool m_Blink;
        LEDBlink_t m_BlinkRateOn;
        LEDBlink_t m_BlinkRateOff;

    };

//    typedef struct
//    {
//        uint8_t color;
//        uint8_t blinkRateOn;
//        uint8_t blinkRateOff;

//    } PowerLedCtrl_t;


    //! \brief Enum describing LED modes
    typedef enum
    {
        LED_ALL_OFF,
        LED_RED_ON,
        LED_RED_GREEN_ON,
        LED_RED_GREEN_BLUE_ON,
        LED_RED_BLUE_ON,
        LED_GREEN_ON,
        LED_GREEN_BLUE_ON,
        LED_BLUE_ON
    } WIFILEDRGB_t;

    //! \brief Enum describing WIFILEDs and colors
    typedef enum
    {
        LED_WIFI_RED,
        LED_WIFI_GREEN,
        LED_WIFI_BLUE,
    } WifiLedColor_t;


    //! \brief Sets the power LED
    //! \param PowerLedCtrl_t : Mode for the power LED
    void setPowerLED(PowerLedCtrl_t a_mode);

    //! \brief Sets the wifi LED
    //! \param WIFILEDRGB_t : Mode for the power LED
    void setWifiLED(WIFILEDRGB_t a_LEDRGB);

    //! \brief Sets a LED
    //! \param LEDColor_t : LED to set
    //! \param bool : LED state
    void setGpioWifiLed(WifiLedColor_t a_LEDColor, bool a_State);

    //! \brief Gets a LED state
    //! \param LEDColor_t : LED to get
    //! \return bool : LED state
    bool getWifiLED(WifiLedColor_t a_LEDColor);

private:
    void populateMap();
    Types::GPIOFunction_t findWifiLedFunction(WifiLedColor_t a_LEDColor);
    std::string findPowerLedColor(PowerLedColor_t a_LEDColor);

    GPIO m_GPIO;
    std::map<WifiLedColor_t, Types::GPIOFunction_t> m_WIFIledMap;
    std::map<PowerLedColor_t, std::string> m_PowerLedMap;
    File *m_LEDFile;
};

}


#endif // LED_H_
