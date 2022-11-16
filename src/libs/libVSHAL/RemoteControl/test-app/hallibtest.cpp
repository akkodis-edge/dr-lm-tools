#include <iostream>
#include <stdio.h>

#include "hallibtest.h"
#include "hallibtest_common.h"
#include <HALLib.h>
#include <Power.h>


using namespace std;
using namespace LM_VSHAL;


void print_hook(HALLib *halLib)
{
    P_S( POWER_AUDIO,          on_str_,           getPower().getAudioPower() );
    P_S( POWER_WIFI,           on_str_,           getPower().getWifiPower() );
    P_S( POWER_USB,            on_str_,           getPower().getUSBOTGPower() );
    P_N( CONFIG_GET_ALL,       "Get all config params" );
    P_N( CONFIG_GET_PARAM,     "Get config parameter" );
    P_N( CONFIG_SET_PARAM,     "Set config parameter" );
    P_N( CONFIG_DELETE_PARAM,  "Delete config parameter" );
}


int parse_hook(HALLib *halLib, int choice, char letter)
{
    switch( choice )
    {
    case POWER_AUDIO:
        halLib->getPower().setAudioPower( getChoice("OFF[0], ON[1]: ", letter) );
        break;
    case POWER_WIFI:
        halLib->getPower().setWifiPower( getChoice("OFF[0], ON[1]: ", letter) );
        break;
    case POWER_USB:
        cout << "Read only" << endl;
        anyKey();
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
