#ifndef IOCONTROLLER_H_
#define IOCONTROLLER_H_


#include <GPIO.h>


//! \brief Class for the IOController.
//! \brief This class is GPIO based.
namespace LM_VSHAL
{

class IOController
{
public:
    //! \brief Constructor
    //! \param a_Path : The path to gpio is sysfs
    IOController(std::string a_Path);
    //! \brief Destructor
    virtual ~IOController();

    //! \brief Enum describing IO controller boot mode
    typedef enum
    {
        IOCTRLBOOT_NORMAL,
        IOCTRLBOOT_REPROGRAM
    } IOCtrlBootMode_t;

    //! \brief Sets the IO controller boot mode
    //! \param IOCtrlBootMode_t : The IO controller boot mode
    void setBootMode(IOCtrlBootMode_t a_BootMode);

    //! \brief Gets the IO controller boot mode
    //! \return IOCtrlBootMode_t : The IO controller boot mode
    IOCtrlBootMode_t getBootMode();

    //! \brief Resets the IO controller
    //! \param bool : true to reset, false not to reset
    void reset(bool a_Reset);

    //! \brief Reads the IO controller reset state
    //! \note  Useful?
    //! \return bool : true = reset, false = not reset
    bool reset();

private:
    GPIO m_GPIO;
};

}


#endif // IOCONTROLLER_H_
