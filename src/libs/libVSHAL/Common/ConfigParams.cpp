#include <sstream>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/stat.h>

#include "ConfigParams.h"
#include <HALException.h>


using namespace LM_VSHAL;
using namespace std;


pthread_mutex_t global_config_mutex = PTHREAD_MUTEX_INITIALIZER;



ConfigParams::ConfigParams(const string& a_Path)
: m_Lock(global_config_mutex),
  m_ConfigPath(a_Path)
{
    struct stat stFileInfo;

    if( stat(a_Path.c_str(), &stFileInfo) != 0 )
    {
        throw HALException( "ConfigParams: Path does not exist: " + a_Path );
    }
}


ConfigParams::~ConfigParams()
{
}


bool ConfigParams::setKey(string m_Key, string m_Value)
{
    vector<string> config = readConfig(m_ConfigPath);

    vector<string>::iterator it;
    for ( it=config.begin() ; it < config.end(); it++ )
    {
        if( m_Key == (*it).substr( 0, (*it).find_first_of( "=" )) )
        {
            *it = m_Key + "=" + m_Value;

            writeConfig(m_ConfigPath, config);
            return true;
        }
    }

    return createKey(m_Key, m_Value);
}


bool ConfigParams::deleteKey(string m_Key)
{
    vector<string> config = readConfig(m_ConfigPath);

    vector<string>::iterator it;
    for ( it=config.begin() ; it < config.end(); it++ )
    {
        if( m_Key == (*it).substr( 0, (*it).find_first_of( "=" )) )
        {
            config.erase(it);

            writeConfig(m_ConfigPath, config);
            return true;
        }
    }

    return false;
}


bool ConfigParams::createKey(string m_Key, string m_Value)
{
    vector<string> config = readConfig(m_ConfigPath);

    string key = m_Key + "=" + m_Value;
    config.push_back( key );

    return writeConfig(m_ConfigPath, config);
}


bool ConfigParams::getKey(string m_Key, string &m_Value)
{
    vector<string> config = readConfig(m_ConfigPath);

    vector<string>::iterator it;
    for ( it=config.begin() ; it < config.end(); it++ )
        if( m_Key == (*it).substr( 0, (*it).find_first_of( "=" )) )
        {
            m_Value = (*it).substr( (*it).find_first_of( "=" )+1 );

            return true;
        }

    return false;
}


vector<string> ConfigParams::getKeys()
{
    vector<string> config = readConfig(m_ConfigPath);
    vector<string> keys;

    vector<string>::iterator it;
    for ( it=config.begin() ; it < config.end(); it++ )
        keys.push_back( (*it).substr( 0, (*it).find_first_of( "=" )) );

    return keys;
}


vector<string> ConfigParams::readConfig(string a_Path)
{
    vector<string> config;

    m_Lock.lock();

    ifstream ifs( a_Path.c_str(), ios_base::in );
    ifs.seekg(CONFIG_OFFSET);

    uint8_t eeprom[4000]; //ram copy of eeprom
    memset(eeprom, 0, 4000);
    size_t position=CONFIG_OFFSET;
    uint8_t character = 0;
    uint32_t checksum = 0;
    while (ifs.good() && position <= CONFIG_OFFSET+CONFIG_LENGTH)
    {

        character = (char)ifs.get();
        if (ifs.good())
        {
            eeprom[position++] = character;
        }
    }
    uint32_t storedChecksum = (eeprom[3] << 24) | (eeprom[2] << 16) | (eeprom[1] << 8) | (eeprom[0]);
    checksum = computeCRC(0,&eeprom[4],CONFIG_LENGTH - 4);

    if (storedChecksum != checksum)
    {
        throw HALException( "Incorrect checksum");
    }

    size_t startOfString=CONFIG_OFFSET + 4;

    if('\0' != eeprom[startOfString]) //If \0 in first position, config is empty
    {
        for (size_t n=CONFIG_OFFSET + 4; n<position; ++n)
        {
            if ('\0' == eeprom[n]) // if \0 then end of string.
            {
                string temp((char*)&eeprom[startOfString]);
                config.push_back(temp);
                startOfString = n+1;
                if ('\0' == eeprom[startOfString]) //Check if next character is also \0, if so our memory block does not contain any more keys.
                {
                    break;
                }
            }
        }
    }

    m_Lock.unlock();

    return config;
}


bool ConfigParams::writeConfig(string a_Path, vector<string> &a_Config)
{
    streamoff lenght;
    char *buffer;

    m_Lock.lock();

    ifstream ifs( a_Path.c_str(), ios_base::in | ios_base::binary );

    ifs.seekg( 0, ios::end );
    lenght = ifs.tellg();
    ifs.seekg( 0, ios::beg );

    buffer = new char [lenght];

    ifs.read( buffer, (int)lenght );
    ifs.close();

    memset( buffer+CONFIG_OFFSET, 0x0, (int)lenght-CONFIG_OFFSET);

    ofstream ofs( a_Path.c_str(), ios_base::out | ios_base::binary );

    filebuf *outbuf = ofs.rdbuf();
    for( int i = 0; i<lenght; i++ )
    {
        outbuf->sputc(buffer[i]);
    }
    uint32_t checksum = 0;
    uint32_t position = CONFIG_OFFSET+4; //skip first 4 bytes as these are reserved for checksum

    vector<string>::iterator it;
    for ( it=a_Config.begin() ; it < a_Config.end(); it++ )
    {
        memcpy(&buffer[position], (*it).c_str(),(*it).length());
        position += (*it).length() + 1;
    }
    checksum = computeCRC(0,(uint8_t *) &buffer[CONFIG_OFFSET+4],2044);
    buffer[CONFIG_OFFSET] = checksum & 0xff;
    buffer[CONFIG_OFFSET+1] = (checksum >> 8) & 0xff;
    buffer[CONFIG_OFFSET+2] = (checksum >> 16) & 0xff;
    buffer[CONFIG_OFFSET+3] = (checksum >> 24) & 0xff;

    ofs.seekp(CONFIG_OFFSET);
    for( int i = 0; i<lenght; i++ )
    {
        outbuf->sputc(buffer[i]);
    }

    ofs.close();

    m_Lock.unlock();

    delete[] buffer;

    return true;
}

bool ConfigParams::InitialiseConfig()
{
    int32_t lenght = CONFIG_LENGTH;
    char *buffer;

    m_Lock.lock();

    buffer = new char [lenght];

    memset( buffer+CONFIG_OFFSET, 0x0, (int)lenght-CONFIG_OFFSET);

    ofstream ofs( m_ConfigPath.c_str(), ios_base::out | ios_base::binary );

    filebuf *outbuf = ofs.rdbuf();
    uint32_t checksum = 0;

    checksum = computeCRC(0,(uint8_t *) &buffer[CONFIG_OFFSET+4],2044);
    buffer[CONFIG_OFFSET] = checksum & 0xff;
    buffer[CONFIG_OFFSET+1] = (checksum >> 8) & 0xff;
    buffer[CONFIG_OFFSET+2] = (checksum >> 16) & 0xff;
    buffer[CONFIG_OFFSET+3] = (checksum >> 24) & 0xff;

    for( int i = 0; i<lenght; i++ )
    {
        outbuf->sputc(buffer[i]);
    }

    ofs.close();

    delete[] buffer;

    m_Lock.unlock();

    vector<string> config;
    bool result = false;

    //Check if succeeded
    try
    {
        config = readConfig(m_ConfigPath);
        if(config.size() == 0)
        {
            result = true;
        }


    }

    catch(LM_VSHAL::HALException e)
    {
        throw HALException( "ConfigParams: Error initialising config. Error: " + e.description());
    }


    return result;
}
