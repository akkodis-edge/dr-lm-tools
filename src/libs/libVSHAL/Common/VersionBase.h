#ifndef VERSIONBASE_H_
#define VERSIONBASE_H_


#include <string>


namespace LM_VSHAL
{

//! \brief Virtual version base class. Must be implemented in BU/RC
class VersionBase
{
public:
    //! \brief Constructor
    VersionBase() {};
    //! \brief Destructor
    virtual ~VersionBase() {};

    //! \brief Enum describing architecture
    typedef enum
    {
        VARIANT_OMAP,
        VARIANT_NULL,
        VARIANT_WIN32,
        VARIANT_LINUX
    } Architecture_t;

    //! \brief Enum describing system variant
    typedef enum
    {
        SYSTEM_BU,
        SYSTEM_RC,
    } System_t;

    //! \brief Virtual method to be implemented in parent class
    virtual int versionMajor() = 0;
    //! \brief Virtual method to be implemented in parent class
    virtual int versionMinor() = 0;
    //! \brief Virtual method to be implemented in parent class
    virtual Architecture_t architecture() = 0;
    //! \brief Virtual method to be implemented in parent class
    virtual std::string architectureDesc() = 0;
    //! \brief Virtual method to be implemented in parent class
    virtual System_t system() = 0;
    //! \brief Virtual method to be implemented in parent class
    virtual std::string systemDesc() = 0;

protected:
    static const unsigned int VERSION_MAJOR = 0;
    static const unsigned int VERSION_MINOR = 1;
};

}

#endif // VERSIONBASE_H_
