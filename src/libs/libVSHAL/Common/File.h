#ifndef FILE_H
#define FILE_H
#include <sys/stat.h>
#include <stdio.h>
#include <string>

#include "Lock.h"

namespace LM_VSHAL
{

//! \brief Class for handling access to sysfs
//! \brief This class is file based.
class File
{
public:
    //! \brief Constructor
    //! \param a_Path : The path to PWM device
    File(std::string a_Prefix);

    //! \brief Opens a function file in sysfs
    //! \param std::string : Function to open
    //! \return FILE* : Pointer file struct
    FILE *open(std::string a_Function, bool a_ReadOnly=true);

    //! \brief Gets value from function file in sysfs
    //! \param std::string : Function to get
    //! \return int : Value
    int get(std::string a_Function);

    //! \brief Gets value from function file in sysfs
    //! \param std::string : Function to get
    //! \return std::string : Value
    std::string getString(std::string a_Function);

    //! \brief Sets string value to function file in sysfs
    //! \param std::string : Function to set
    //! \param std::string : Value to set
    void setString(std::string a_Function, std::string a_Value);

    //! \brief Sets value to function file in sysfs
    //! \param std::string : Function to get
    void set(std::string a_Function, int a_Value);

    static bool exists(std::string a_Path);
private:
    std::string m_Prefix;
    Lock m_Lock;

};

}

#endif // FILE_H
