#ifndef CONFIGPARAMS_H_
#define CONFIGPARAMS_H_


#include <vector>
#include <string>

#include <Lock.h>
#include <cstdint>
#include "HALException.h"
#include "crc32.h"


namespace LM_VSHAL
{

    //! \brief Handles reading and writing of config data to eeprom.
    //! The config data is put in the EEPROM at offset CONFIG_OFFSET.
    //! The config data is stored as a pair of a key and a value.
    //! Key is the name identifier and value is the data part.
    class ConfigParams
    {
        public:
            //! \brief Constructor
            //! \param a_Path : The path to the eeprom is sysfs
            ConfigParams(const std::string& a_Path);
            //! \brief Destructor
            virtual ~ConfigParams();

            //! \brief Sets a key. If the key does not exist, it gets created.
            //! \param m_Key : The key part of the config
            //! \param m_Value : The value part of the config
            //! \return true if successful, false if not
            bool setKey(std::string m_Key, std::string m_Value);

            //! \brief Gets a key.
            //! \param m_Key : The key part of the config
            //! \param m_Value : Reference to value.
            //! \return true if successful, false if not
            bool getKey(std::string m_Key, std::string &m_Value);

            //! \brief Get all keys.
            //! \return Vector containing all key names
            std::vector<std::string> getKeys();

            //! \brief Deletes a key.
            //! \param m_Key : The key part of the config
            //! \return true if successful, false if not
            bool deleteKey(std::string m_Key);

            //! \brief Erases everything in eeprom and sets checksum.
            //! \return true if successful, false if not
            bool InitialiseConfig();

        private:
            Lock m_Lock;
            std::string m_ConfigPath;
            std::vector<std::string> m_Keys;

            std::vector<std::string> readConfig(std::string a_Path);
            bool createKey(std::string m_Key, std::string m_Value);
            bool writeConfig(std::string a_Path, std::vector<std::string> &a_Config);

            static const uint32_t CONFIG_OFFSET = 0;
            static const uint32_t CONFIG_LENGTH = 2048;


    };

}


#endif // CONFIGPARAMS_H_
