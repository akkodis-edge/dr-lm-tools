#ifndef IO_H_
#define IO_H_


#include <GPIO.h>


namespace LM_VSHAL
{

//! \brief Class for various IO.
//! \brief This class is GPIO based.
class IO
{
public:
    //! \brief Constructor
    //! \param a_Path : The path to gpio is sysfs
    IO(std::string a_Path);
    //! \brief Destructor
    virtual ~IO();

    //! \brief Enum describing UART destination
    typedef enum
    {
        CPR_METER,
        BOARD_CONTROLLER
    } UARTDestination_t;

    //! \brief Sets the destination for UART 4
    //! \param a_Destination : Enum describing UART destination
    void setUART4Destination(UARTDestination_t a_Destination);

    //! \brief Gets the destination for UART 4
    //! \return a_Destination : The destination for UART 4
    UARTDestination_t getUART4Destination();

    //! \brief Reads the line in connection (read only)
    //! \return bool : The line in connection
    bool getLineInConnection();

    //! \brief Reads the line out connection (read only)
    //! \return bool : The line out connection
    bool getLineOutConnection();

private:
    GPIO m_GPIO;
};

}


#endif // IO_H_
