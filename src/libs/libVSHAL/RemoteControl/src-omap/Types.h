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
        GPIO_LCDPWR_EN,
        GPIO_LEDPWR_EN,
        GPIO_BST5V_PWREN,
        GPIO_V3V_PWREN,
        GPIO_PHY_PWREN,
        GPIO_PHY_RESET,
        GPIO_LAN_PWREN,
        GPIO_RESET_LAN,
        GPIO_PWREN_TH,
        GPIO_AMP_SDn,
        GPIO_PWREN_WIFI,
        GPIO_OTG5V_EN,
        GPIO_OTG_OC,
        GPIO_RESET_WLANn,
    } GPIOFunction_t;

};

}

#endif // TYPES_H_
