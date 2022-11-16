#ifndef TYPES_H_
#define TYPES_H_


#include "VersionBase.h"


namespace LM_VSHAL
{

//! \brief Common types used by the library
class Types
{
public:

    //! \brief Enum describing GPIO functionality
    typedef enum
    {
        GPIO_UNUSED,
	GPIO_VOCAL_AMP_EN,
        GPIO_5V_MANIKIN_EN,
        GPIO_LINE_IN_DET,
        GPIO_LINE_OUT_DET,
        GPIO_PWRBT,
        GPIO_MCU_BOOT_0,
        GPIO_MCU_BOOT_1,
        GPIO_MCU_RESET,
        GPIO_UART4_SEL,
        GPIO_PWR_LED_R,
        GPIO_PWR_LED_G,
        GPIO_PWR_LED_B,
        GPIO_WIFI_LED_R,
        GPIO_WIFI_LED_G,
        GPIO_WIFI_LED_B,
        GPIO_BOOST_12V_EN,
    } GPIOFunction_t;

};

}

#endif // TYPES_H_
