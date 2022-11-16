#ifndef PWM_H_
#define PWM_H_


#include <stdint.h>
#include <map>

#include <Lock.h>


namespace LM_VSHAL
{

//! \brief Class for PWM based control.
class PWM
{
public:
    //! \brief Constructor
    //! \param a_Path : The path to PWM device
    PWM(std::string a_Path);
    //! \brief Destructor
    virtual ~PWM();

    //! \brief Enum describing PWM functionality
    typedef enum
    {
        PWM_UNUSED,
        PWM_MANIKIN_GPO1,
        PWM_MANIKIN_GPO2,
        PWM_MANIKIN_ECG,
    } PWMFunction_t;

    //! \brief Sets PWM8
    //! \param int8_t : Duty cycle
    void setPWM8(int8_t a_DutyCycle);

    //! \brief Gets PWM8
    //! \return int8_t : Duty cycle
    int8_t getPWM8();

    //! \brief Sets PWM9
    //! \param int8_t : Duty cycle
    void setPWM9(int8_t a_DutyCycle);

    //! \brief Gets PWM9
    //! \return int8_t : Duty cycle
    int8_t getPWM9();

    //! \brief Sets ECG
    //! \param a_DutyCycle : Duty cycle
    void setECGPWM(int8_t a_DutyCycle);

    //! \brief Gets ECG
    //! \return int8_t : Duty cycle
    int8_t getECGPWM();

private:
    void populateMap();
    int findPWM(PWMFunction_t Function);
    FILE *openPWM(PWMFunction_t a_Function);
    void setPWM(PWMFunction_t a_Function, int8_t a_Value);

    Lock m_Lock;
    std::string m_PWMPrefix;
    std::map<PWMFunction_t, int> m_PWMMap;
};

}


#endif // PWM_H_
