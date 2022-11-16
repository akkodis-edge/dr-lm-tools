#ifndef GPIOBASE_H_
#define GPIOBASE_H_


#include <string>
#include <map>

#include <Lock.h>
#include <Types.h>


namespace LM_VSHAL
{

//! \brief Base class. Handles GPIO reading and writing.
class GPIOBase
{
public:
    //! \brief Constructor
    //! \param a_Path : The path to gpio is sysfs
    GPIOBase(std::string a_Path);
    //! \brief Destructor
    virtual ~GPIOBase();

    //! \brief Sets a GPIO pin
    //! \param a_GPIOFunction : Enum describing GPIO functionality
    //! \param m_Value : The desired state of the GPIO pin
    void setGPIO(Types::GPIOFunction_t a_GPIOFunction, bool a_State);

    //! \brief Gets the state of a GPIO pin
    //! \param a_GPIOFunction : Enum describing GPIO functionality
    //! \return The current state of the GPIO pin
    bool getGPIO(Types::GPIOFunction_t a_GPIOFunction);

    //! \brief Gets the GPIO map. This is a functionality/pin map
    //! \return The GPIO map
    std::map<Types::GPIOFunction_t, int> getGPIOMap();

private:
    Lock m_Lock;
    std::string m_GPIOPrefix;

    virtual void populateMap() = 0;
    int findPin(Types::GPIOFunction_t a_GPIOFunction);
    FILE *openGPIO(Types::GPIOFunction_t a_GPIOFunction);

protected:
    std::map<Types::GPIOFunction_t, int> m_GPIOMap;
};

}


#endif // GPIOBASE_H_
