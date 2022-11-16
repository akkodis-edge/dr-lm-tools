#include <iostream>
#include <cstdio>

#include "hallibtest.h"
#include "hallibtest_common.h"
#include <HALLib.h>
#include <LED.h>
#include <Power.h>
#include <IOController.h>
#include <IO.h>


using namespace std;
using namespace LM_VSHAL;


void print_hook(HALLib *halLib)
{
//    P_S( LED_POWER_RED,          on_str_,           getLED().getLED(LED::LED_POWER_RED) );
//    P_S( LED_POWER_GREEN,        on_str_,           getLED().getLED(LED::LED_POWER_GREEN) );
//    P_S( LED_POWER_BLUE,         on_str_,           getLED().getLED(LED::LED_POWER_BLUE) );
    P_S( LED_WIFI_RED,           on_str_,           getLED().getWifiLED(LED::LED_WIFI_RED) );
    P_S( LED_WIFI_GREEN,         on_str_,           getLED().getWifiLED(LED::LED_WIFI_GREEN) );
    P_S( LED_WIFI_BLUE,          on_str_,           getLED().getWifiLED(LED::LED_WIFI_BLUE) );
    P_S( POWER_MANIKIN,          on_str_,           getPower().getManikinPower() );
    P_S( POWER_BUTTON,           on_str_,           getPower().getPowerButton() );
    P_S( IOCONTROLLER_BOOT_MODE, bootmode_str_,     getIOController().getBootMode() );
    P_S( IOCONTROLLER_RESET,     on_str_,           getIOController().reset() );
    P_S( IO_UART4_SEL,           uart_select_str_,  getIO().getUART4Destination() );
    P_S( IO_LINE_IN_DET,         on_str_,           getIO().getLineInConnection() );
    P_S( IO_LINE_OUT_DET,        on_str_,           getIO().getLineOutConnection() );
    P_N( CONFIG_GET_ALL,         "Get all config params" );
    P_N( CONFIG_GET_PARAM,       "Get config parameter" );
    P_N( CONFIG_SET_PARAM,       "Set config parameter" );
    P_N( CONFIG_DELETE_PARAM,    "Delete config parameter" );
}


int parse_hook(HALLib *halLib, int choice, char letter)
{
    switch( choice )
    {
    case LED_POWER_RED:
        //halLib->getLED().setPowerLED( LED::LED_POWER_RED, getChoice("OFF[0], ON[1]: ", letter) );
        break;
    case LED_POWER_GREEN:
        //halLib->getLED().setPowerLED( LED::LED_POWER_GREEN, getChoice("OFF[0], ON[1]: ", letter) );
        break;
    case LED_POWER_BLUE:
        //halLib->getLED().setPowerLED( LED::LED_POWER_BLUE, getChoice("OFF[0], ON[1]: ", letter) );
        break;
    case LED_WIFI_RED:
        halLib->getLED().setGpioWifiLed( LED::LED_WIFI_RED, getChoice("OFF[0], ON[1]: ", letter) );
        break;
    case LED_WIFI_GREEN:
        halLib->getLED().setGpioWifiLed( LED::LED_WIFI_GREEN, getChoice("OFF[0], ON[1]: ", letter) );
        break;
    case LED_WIFI_BLUE:
        halLib->getLED().setGpioWifiLed( LED::LED_WIFI_BLUE, getChoice("OFF[0], ON[1]: ", letter) );
        break;
    case POWER_MANIKIN:
        halLib->getPower().setManikinPower( getChoice("OFF[0], ON[1]: ", letter) );
        break;
    case POWER_BUTTON:
    case IO_LINE_IN_DET:
    case IO_LINE_OUT_DET:
        cout << "Read only" << endl;
        anyKey();
        break;
    case IOCONTROLLER_BOOT_MODE:
        halLib->getIOController().setBootMode( static_cast<IOController::IOCtrlBootMode_t>(getChoice("Normal[0], Reprogram[1]: ", letter)) );
        break;
    case IOCONTROLLER_RESET:
        halLib->getIOController().reset( getChoice("Reset[0], No Reset[1]: ", letter) );
        break;
    case IO_UART4_SEL:
        halLib->getIO().setUART4Destination( static_cast<IO::UARTDestination_t>(getChoice("CPR Meter[0], Board Controller[1]: ", letter)) );
        break;
    case CONFIG_GET_ALL:
        {
            std::vector<std::string> keys = halLib->getConfigParams().getKeys();
            vector<std::string>::iterator it;
            for ( it=keys.begin() ; it < keys.end(); it++ )
            {
                string value;
                halLib->getConfigParams().getKey(*it, value);
                cout << *it << "=" << value << endl;
            }
        }
        anyKey();
        break;
    case CONFIG_GET_PARAM:
        {
            cout << "Config paremeter: ";
            cout.flush();
            string key;
            string value;
            cin >> key;

            if( halLib->getConfigParams().getKey(key, value) )
                cout << key << "=" << value << endl;
            else
                cout << "Not found" << endl;

            cin.get();
            anyKey();
        }
        break;
    case CONFIG_SET_PARAM:
        {
            cout << "Config paremeter: ";
            cout.flush();
            string key;
            string value;
            cin >> key;

            if( halLib->getConfigParams().getKey(key, value) )
                cout << key << "=" << value << endl;
            else
            {
                cout << "Not found, creating new" << endl;
            }

            cout << "New value: ";
            cout.flush();
            cin >> value;
            cout << "writing ... " << endl;
            halLib->getConfigParams().setKey(key, value);

            cin.get();
            anyKey();
        }
        break;
    case CONFIG_DELETE_PARAM:
        {
            cout << "Config paremeter: ";
            cout.flush();
            string key;
            string value;
            cin >> key;

            if( halLib->getConfigParams().getKey(key, value) )
                cout << key << "=" << value << endl;
            else
            {
                cout << "Not found" << endl;
                cin.get();
                anyKey();
                break;
            }

            cin.get();
            if( getChoice("Delete? YES[1], NO[0]: ", letter) )
            {
                cout << endl << "deleting ... " << endl;
                halLib->getConfigParams().deleteKey(key);
            }
        }
        break;
    case ESC:
        return 1;
    default:
        break;
    }

    return 0;
}
