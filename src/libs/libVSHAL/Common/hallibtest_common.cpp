#include <iostream>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "hallibtest_common.h"
#include <HALLib.h>
#include <HALException.h>
#include "getconfigpath.h"

using namespace std;
using namespace LM_VSHAL;


int menu_main(string gpioPath, string pwmPath, string configPath);

extern void print_hook(HALLib *halLib);
extern int parse_hook(HALLib *halLib, int choice, char letter);


void printUsage(const char *name)
{
    printf("Usage: %s [OPTION]\n\n", name);
    printf("Options:\n");
    printf("  -g            GPIO path\n");
    printf("  -p            PWM path\n");
    printf("  -c            config path\n");
    printf("  -c            config path\n");
    printf("  -cg           get config parameter\n");
    printf("  -cs           set config parameter\n");
    printf("  -cv           parameter value. Used with -cs\n");
    printf("\nIf no paths are specified, defaults are used\n\n");
}


bool  isNamePresent( int argc, char *argv[], const char *name)
{
    for (int i = 1; i < argc; i++)
    {
        if ( 0 == strcmp(argv[i], name))
        {
            return true;
        }
    }

    return false;
}


char *getArgString( int argc, char *argv[], const char* name)
{
    for (int i = 1; i < argc - 1; i++)
    {
        if ( 0 == strcmp(argv[i], name))
        {
            return argv[i+1];
        }
    }

    return 0;
}


int getChoice(const char *message, char &letter)
{
    bool run = true;
    int ch = 0;
    int numChars = 0;
    char temp[6];

    memset(temp, 0, 5);
    cout << message;
    cout.flush();
    while(run)
    {
        struct termios oldt, newt;

        tcgetattr( STDIN_FILENO, &oldt );
        newt = oldt;
        newt.c_lflag &= ~( ICANON | ECHO );
        tcsetattr( STDIN_FILENO, TCSANOW, &newt );
        ch = getchar();
        tcsetattr( STDIN_FILENO, TCSANOW, &oldt );

        if( ch==27 )
            return ESC;
        else if( ch==8 )
        {
            // Backspace
            numChars--;
            temp[numChars] = 0;
            putchar(ch);
            putchar(32);
            putchar(ch);
            continue;
        }

        temp[numChars] = (char)ch;
        numChars++;
        if( (ch>=97) && ((ch<=122)) )
        {
            letter = (char)ch;
            putchar(ch);
            return ASCII;
        }

        if( ch==10 )
            break;

        putchar(ch);
        if( numChars >= 5 )
            run = false;
    }

    int ret = 0;
    sscanf(temp, "%d", &ret);

    return ret;
}


int menu_main(string gpioPath, string pwmPath, string configPath)
{
    HALLib *halLib = NULL;

    if( (gpioPath != "") && (pwmPath != "") && (configPath != "") )
        halLib= new HALLib(gpioPath, pwmPath, configPath);
    else
        halLib= new HALLib();

    while(1)
    {
        printf( "----------------------------------------------------\n" );
        printf( "  VitalSim II HAL\n");
        printf( "----------------------------------------------------\n" );
        printf( "  System:  %s\n", halLib->getVersion().systemDesc().c_str() );
        printf( "  Variant: %s\n", halLib->getVersion().architectureDesc().c_str() );
        printf( "  Version: %02d.%02d\n", halLib->getVersion().versionMajor(), halLib->getVersion().versionMinor() );
        printf( "----------------------------------------------------\n" );
        printf( "\n");

        print_hook(halLib);

        printf( "\n" );
        cout << " Esc) Exit" << endl;

        char letter = 0;
        int choice = getChoice("\nSelect: ", letter);
        printf( "\n" );

        if( parse_hook(halLib, choice, letter) )
            break;

        printf( "\n" );
    }

    printf( "\n" );

    delete halLib;

    return 0;
}


void anyKey(void)
{
    struct termios oldt, newt;
    int ch = 0;

    printf("\nPress any key ...");
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
}


int main(int argc, char *argv[])
{
    int ret = 1;
    char *argString = 0;

    string gpioPath = GPIO_PATH;
    string pwmPath = PWM_PATH;
    string configPath = getConfigPath();

    if( isNamePresent( argc, argv, "--help") )
    {
        printUsage(argv[0]);

        return 0;
    }
    if( (argString = getArgString( argc, argv, "-g")) != 0 )
    {
        gpioPath = argString;
    }
    if( (argString = getArgString( argc, argv, "-p")) != 0 )
    {
        pwmPath = argString;
    }
    if( (argString = getArgString( argc, argv, "-c")) != 0 )
    {
        configPath = argString;
    }

    try
    {
        if( (argString = getArgString( argc, argv, "-cg")) != 0 )
        {
            HALLib *halLib = new HALLib(gpioPath, pwmPath, configPath);
            string value;

            if( halLib->getConfigParams().getKey(argString, value) )
                cout << value << endl;
            else
                cout << "Not found" << endl;

            delete halLib;

            return 0;
        }
        else if( (argString = getArgString( argc, argv, "-cs")) != 0 )
        {
            string key = argString;
            if( (argString = getArgString( argc, argv, "-cv")) == 0 )
            {
                cout << "Parameter -cv must be specified" << endl;

                return 1;
            }

            string value = argString;
            HALLib *halLib = new HALLib(gpioPath, pwmPath, configPath);
            cout << "writing ... " << endl;
            if( !halLib->getConfigParams().setKey(key, value) )
                cout << "Not found" << endl;

            delete halLib;

            return 0;
        }

        ret = menu_main(gpioPath, pwmPath, configPath);
    }
    catch (HALException &e)
    {
        printf( "HalException: %s\n", e.description().c_str() );
    }

    return ret;
}
