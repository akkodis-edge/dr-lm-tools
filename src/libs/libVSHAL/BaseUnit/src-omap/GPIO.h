#ifndef GPIO_H_
#define GPIO_H_


#include <string>

#include <GPIOBase.h>


//! \brief Handles GPIO reading and writing.
//! Several classes has this class as a member.
namespace LM_VSHAL
{

class GPIO : public GPIOBase
{
public:
    //! \brief Constructor
    //! \param a_Path : The path to gpio is sysfs
    GPIO(std::string a_Path);
    //! \brief Destructor
    virtual ~GPIO();

private:
    void populateMap();
};

}


#endif /* GPIO_H_ */
