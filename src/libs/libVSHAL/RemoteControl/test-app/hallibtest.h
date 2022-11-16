#ifndef HALLIBTEST_H_
#define HALLIBTEST_H_


#include <string>


const char bootmode_str_[2][20] =
{
        "Normal", "Reprogram"
};


const char uart_select_str_[2][20] =
{
        "CPR Meter", "Board Controller"
};


typedef enum
{
    NOTHING = 0,
    POWER_AUDIO,
    POWER_WIFI,
    POWER_USB,
    CONFIG_GET_ALL,
    CONFIG_GET_PARAM,
    CONFIG_SET_PARAM,
    CONFIG_DELETE_PARAM,
} Choices_t;


typedef struct
{
    Choices_t choice;
    std::string text;
} menu_t;


menu_t menu_[] =
{
        {NOTHING, "Nothing"},
        {POWER_AUDIO, "POWER_AUDIO"},
        {POWER_WIFI, "POWER_WIFI"},
        {POWER_USB, "POWER_USB"},
        {CONFIG_GET_ALL, "CONFIG_GET_ALL"},
        {CONFIG_GET_PARAM, "CONFIG_GET_PARAM"},
        {CONFIG_SET_PARAM, "CONFIG_SET_PARAM"},
        {CONFIG_DELETE_PARAM, "CONFIG_DELETE_PARAM"},
};


#endif // HALLIBTEST_H_
