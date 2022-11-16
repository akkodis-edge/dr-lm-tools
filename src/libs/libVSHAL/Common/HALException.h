#ifndef HALEXCEPTION_H_
#define HALEXCEPTION_H_


#include <string>

//! \brief The namespace used througout the LM_VSHAL lib
namespace LM_VSHAL
{

//! \brief Exception class used in the LM_VSHAL lib. Code using this library should catch this exception.
class HALException
{
public:
    //! \brief Constructor
    //! \param s : String describing the exception
    HALException(std::string s) :text_(s) { }
    //! \brief Destructor
    virtual ~HALException() {}

    //! \brief Exception description
    //! \param String describing the exception
    std::string description()	{ return text_; }

private:
    std::string text_;
};

}

#endif // HALEXCEPTION_H_
