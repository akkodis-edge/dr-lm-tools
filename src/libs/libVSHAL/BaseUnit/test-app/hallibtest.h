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
    LED_POWER_RED,
    LED_POWER_GREEN,
    LED_POWER_BLUE,
    LED_WIFI_RED,
    LED_WIFI_GREEN,
    LED_WIFI_BLUE,
    POWER_MANIKIN,
    POWER_BUTTON,
    IOCONTROLLER_BOOT_MODE,
    IOCONTROLLER_RESET,
    IO_UART4_SEL,
    IO_LINE_IN_DET,
    IO_LINE_OUT_DET,
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
        {LED_POWER_RED, "LED_POWER_RED"},
        {LED_POWER_GREEN, "LED_POWER_GREEN"},
        {LED_POWER_BLUE, "LED_POWER_BLUE"},
        {LED_WIFI_RED, "LED_WIFI_RED"},
        {LED_WIFI_GREEN, "LED_WIFI_GREEN"},
        {LED_WIFI_BLUE, "LED_WIFI_BLUE"},
        {POWER_MANIKIN, "POWER_MANIKIN"},
        {POWER_BUTTON, "POWER_BUTTON"},
        {IOCONTROLLER_BOOT_MODE, "IOCONTROLLER_BOOT_MODE"},
        {IOCONTROLLER_RESET, "IOCONTROLLER_RESET"},
        {IO_UART4_SEL, "IO_UART4_SEL"},
        {IO_LINE_IN_DET, "IO_LINE_IN_DET"},
        {IO_LINE_OUT_DET, "IO_LINE_OUT_DET"},
        {CONFIG_GET_ALL, "CONFIG_GET_ALL"},
        {CONFIG_GET_PARAM, "CONFIG_GET_PARAM"},
        {CONFIG_SET_PARAM, "CONFIG_SET_PARAM"},
        {CONFIG_DELETE_PARAM, "CONFIG_DELETE_PARAM"},
};


#endif // HALLIBTEST_H_
