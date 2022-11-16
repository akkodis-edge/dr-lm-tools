#ifndef HALLIBBASE_H_
#define HALLIBBASE_H_


#include "Version.h"
#include "ConfigParams.h"



//! \brief Default GPIO path
#define GPIO_PATH           "/sys/class/gpio"
//! \brief Default PWM path
#define PWM_PATH            "/dev/omap_pwm"
//! \brief Default LCD Path
#define LCD_PATH            "/sys/class/backlight/pwm-backlight"
//! \brief Default Graphics device Path
#define GRAPHICS_DEV_PATH   "/sys/class/graphics/fb0"

//! \brief Default Battery devices path
#define BATTERY_DEV_PATH "/sys/class/power_supply"

//! \brief Default Battery realtime info path
#define BATTERY_RT_INFO_PATH "/sys/bus/i2c/devices/3-0078"

//! \brief Default Battery device path for RC
#define BATTERY_DEV_PATH_RC "/sys/class/power_supply/rc-battery-0"

//! \brief Default Battery device path for BU
#define BATTERY_DEV_PATH_BU "/sys/class/power_supply/ds2781-battery.0"

//! \brief Default Battery realtime info path for BU
#define BATTERY_RT_INFO_PATH_BU  "/sys/bus/i2c/devices/3-0078"
#define BATTERY1_RT_INFO_FN_BU "battery1_present"
#define BATTERY2_RT_INFO_FN_BU "battery2_present"

//! \brief Default Battery realtime control path for BU
#define BATTERY1_RT_CONTROL_PATH_BU "/sys/bus/w1/devices/w1_bus_master1"
#define BATTERY2_RT_CONTROL_PATH_BU "/sys/bus/w1/devices/w1_bus_master2"

//! \brief Default config (eeprom) path
#define CONFIG_PATH_BU  "/sys/devices/platform/omap/omap_i2c.1/i2c-1/1-0050/eeprom"
#define CONFIG_PATH_RC  "/sys/devices/platform/omap/omap_i2c.3/i2c-3/3-0050/eeprom"

//! \brief Default LED control path
#define LED_CONTROL_PATH_RC "/sys/class/power_supply/rc-battery-0/device"
#define LED_CONTROL_PATH_BU "/sys/class/i2c-adapter/i2c-3/3-0078"

namespace LM_VSHAL
{

//! \brief Base class to be used by BU/RC
class HALLibBase
{
public:
    //! \brief Constructor
    HALLibBase();
    //! \brief Destructor
    virtual ~HALLibBase();

    //! \brief Get SW version class
    Version &getVersion();

    //! \brief Get config param class
    ConfigParams &getConfigParams();

protected:
    Version *m_Version;
    ConfigParams *m_ConfigParams;
};

}


#endif // HALLIBBASE_H_
