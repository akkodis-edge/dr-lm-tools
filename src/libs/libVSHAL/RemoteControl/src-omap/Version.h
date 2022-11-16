#ifndef VERSION_H_
#define VERSION_H_


#include "VersionBase.h"


namespace LM_VSHAL
{

//! \brief Class for library version
class Version : public VersionBase
{
public:
    //! \brief Constructor
    Version() {};
    //! \brief Destructor
    virtual ~Version() {};

    //! \brief Returns Major version number (MAJOR.minor)
    //! \return int : Major version number
    int versionMajor();

    //! \brief Returns Minor version number (major.MINOR)
    //! \return int : Minor version number
    int versionMinor();

    //! \brief Returns enum describing architecture
    //! \return Variant_t : System architecture
    Architecture_t architecture();

    //! \brief Returns system architecture description
    //! \return std::string : System architecture description
    std::string architectureDesc();

    //! \brief Returns enum describing system
    //! \return std::string : System
    System_t system();

    //! \brief Returns system description
    //! \return std::string : System description
    std::string systemDesc();
};

}

#endif // VERSION_H_
